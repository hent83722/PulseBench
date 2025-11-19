#!/usr/bin/env python3
"""
PulseBench GUI

Simple PySide6 GUI that starts/stops the CLI binary and shows live system metrics.
"""
import sys
import os
import signal
import subprocess
import time
import csv
from datetime import datetime
from collections import deque

from PySide6 import QtCore, QtWidgets, QtGui
import pyqtgraph as pg
import psutil


BINARY_PATH = os.path.join(os.path.dirname(os.path.dirname(__file__)), "build", "pulsebench")

UPDATE_INTERVAL_MS = 500
MAX_POINTS = 300

class MetricsWorker(QtCore.QObject):
    metrics_ready = QtCore.Signal(dict)

    def __init__(self, parent=None):
        super().__init__(parent)
        self._timer = QtCore.QTimer(self)
        self._timer.setInterval(UPDATE_INTERVAL_MS)
        self._timer.timeout.connect(self.collect)

    def start(self):
        self._timer.start()

    def stop(self):
        self._timer.stop()

    def collect(self):
        data = {}
        try:
            cpu_percent = psutil.cpu_percent(interval=None)
            mem = psutil.virtual_memory()
            freqs = psutil.cpu_freq(percpu=False)
            temps = None
            try:
                temps = psutil.sensors_temperatures()
            except Exception:
                temps = None

            data = {
                "cpu_percent": cpu_percent,
                "memory_percent": mem.percent,
                "cpu_freq": freqs.current if freqs else None,
                "temps": temps,
                "timestamp": time.time(),
            }
        except Exception as e:
            data = {"error": str(e)}
        self.metrics_ready.emit(data)

class MainWindow(QtWidgets.QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("PulseBench GUI")
        self.resize(1000, 600)

        central = QtWidgets.QWidget()
        self.setCentralWidget(central)

        layout = QtWidgets.QVBoxLayout(central)

        controls = QtWidgets.QHBoxLayout()
        self.binary_path_edit = QtWidgets.QLineEdit(BINARY_PATH)
        self.binary_browse = QtWidgets.QPushButton("Browse...")
        self.start_btn = QtWidgets.QPushButton("Start")
        self.stop_btn = QtWidgets.QPushButton("Stop")
        self.stop_btn.setEnabled(False)
        controls.addWidget(QtWidgets.QLabel("Binary:"))
        controls.addWidget(self.binary_path_edit)
        controls.addWidget(self.binary_browse)
        controls.addWidget(self.start_btn)
        controls.addWidget(self.stop_btn)

        layout.addLayout(controls)


        self.plot_widget = pg.GraphicsLayoutWidget()
        layout.addWidget(self.plot_widget, 1)

        self.cpu_plot = self.plot_widget.addPlot(title="CPU %")
        self.cpu_plot.showGrid(x=True, y=True)
        self.cpu_curve = self.cpu_plot.plot(pen=pg.mkPen("r", width=2))

        self.plot_widget.nextRow()
        self.mem_plot = self.plot_widget.addPlot(title="Memory %")
        self.mem_plot.showGrid(x=True, y=True)
        self.mem_curve = self.mem_plot.plot(pen=pg.mkPen("b", width=2))

        self.plot_widget.nextRow()
        self.freq_plot = self.plot_widget.addPlot(title="CPU Frequency (MHz)")
        self.freq_plot.showGrid(x=True, y=True)
        self.freq_curve = self.freq_plot.plot(pen=pg.mkPen("g", width=2))

        self.plot_widget.nextRow()
        self.temp_plot = self.plot_widget.addPlot(title="CPU Temperature (°C)")
        self.temp_plot.showGrid(x=True, y=True)
        self.temp_curve = self.temp_plot.plot(pen=pg.mkPen("m", width=2))


        self.times = deque(maxlen=MAX_POINTS)
        self.cpu_data = deque(maxlen=MAX_POINTS)
        self.mem_data = deque(maxlen=MAX_POINTS)
        self.freq_data = deque(maxlen=MAX_POINTS)
        self.temp_data = deque(maxlen=MAX_POINTS)


        self.status = self.statusBar()

        self.log = QtWidgets.QTextEdit()
        self.log.setReadOnly(True)
        layout.addWidget(QtWidgets.QLabel("Process output:"))
        layout.addWidget(self.log, 0)

        self.process = None
 
        self.process_watcher = QtCore.QTimer(self)
        self.process_watcher.setInterval(500)
        self.process_watcher.timeout.connect(self._check_process)

      
        self.log_records = []
        self._prompted_save = False
        

       
        self.worker = MetricsWorker()
        self.worker_thread = QtCore.QThread(self)
        self.worker.moveToThread(self.worker_thread)
        self.worker.metrics_ready.connect(self.on_metrics)
        self.worker_thread.started.connect(self.worker.start)
        self.worker_thread.start()

        self.start_btn.clicked.connect(self.start_binary)
        self.stop_btn.clicked.connect(self.stop_binary)
        self.binary_browse.clicked.connect(self.browse_binary)


        app = QtWidgets.QApplication.instance()
        if app:
            app.aboutToQuit.connect(self._shutdown)

    def _shutdown(self):
        self.worker.stop()
        self.worker_thread.quit()
        self.worker_thread.wait(1000)
        if self.process:
            self.stop_binary()

    def browse_binary(self):
        path, _ = QtWidgets.QFileDialog.getOpenFileName(self, "Select binary", os.path.dirname(self.binary_path_edit.text()))
        if path:
            self.binary_path_edit.setText(path)

    @QtCore.Slot(dict)
    def on_metrics(self, data):
        if "error" in data:
            self.status.showMessage(f"Metrics error: {data['error']}")
            return
        t = data.get("timestamp", time.time())
        self.times.append(t)
        self.cpu_data.append(data.get("cpu_percent", 0.0))
        self.mem_data.append(data.get("memory_percent", 0.0))
        self.freq_data.append(data.get("cpu_freq") or 0.0)

   
        temps = data.get("temps")
        temp_val = None
        if temps:
    
            values = []
            for name, entries in temps.items():
                for e in entries:
                    try:
                        if hasattr(e, 'current') and e.current is not None:
                            values.append(float(e.current))
                    except Exception:
                        pass
            if values:
                temp_val = max(values)
        if temp_val is None:
            temp_val = 0.0
        self.temp_data.append(temp_val)

     
        self.log_records.append({
            'timestamp': datetime.fromtimestamp(t).isoformat(),
            'cpu_percent': data.get('cpu_percent', 0.0),
            'memory_percent': data.get('memory_percent', 0.0),
            'cpu_freq': data.get('cpu_freq') or 0.0,
            'cpu_temp': temp_val,
        })

    
        if self.times:
            x = list(range(-len(self.times)+1, 1))
            self.cpu_curve.setData(x, list(self.cpu_data))
            self.mem_curve.setData(x, list(self.mem_data))
            self.freq_curve.setData(x, list(self.freq_data))
            self.temp_curve.setData(x, list(self.temp_data))

    def start_binary(self):
        if self.process:
            self.status.showMessage("Process already running")
            return
        path = self.binary_path_edit.text().strip() or BINARY_PATH
        if not os.path.isfile(path) or not os.access(path, os.X_OK):
            QtWidgets.QMessageBox.warning(self, "Binary missing", f"Binary not found or not executable:\n{path}")
            self.status.showMessage("Binary missing")
            return

        try:
           
            self.process = subprocess.Popen([path], stdout=subprocess.PIPE, stderr=subprocess.PIPE, preexec_fn=os.setsid)
            self.start_btn.setEnabled(False)
            self.stop_btn.setEnabled(True)
         
            self.process_watcher.start()
            self.status.showMessage(f"Started {path} (pid={self.process.pid})")
        except Exception as e:
            QtWidgets.QMessageBox.critical(self, "Start failed", str(e))
            self.status.showMessage("Start failed")

    def stop_binary(self):
        if not self.process:
            self.status.showMessage("No process")
            return
        try:
    
            os.killpg(os.getpgid(self.process.pid), signal.SIGTERM)
            self.process.wait(timeout=3)
        except Exception:
            try:
                self.process.kill()
            except Exception:
                pass
        finally:
       
            try:
                if hasattr(self, 'stdout_thread') and self.stdout_thread.isRunning():
                    self.stdout_thread.quit()
                    self.stdout_thread.wait(500)
                if hasattr(self, 'stderr_thread') and self.stderr_thread.isRunning():
                    self.stderr_thread.quit()
                    self.stderr_thread.wait(500)
            except Exception:
                pass
            self.status.showMessage("Stopped process")
            self.process = None
            self.start_btn.setEnabled(True)
            self.stop_btn.setEnabled(False)
       
            try:
                self.process_watcher.stop()
            except Exception:
                pass
        
            if self.log_records and not self._prompted_save:
                self._prompted_save = True
                save = QtWidgets.QMessageBox.question(self, "Export CSV", "Process ended — export logged metrics to CSV?", QtWidgets.QMessageBox.Yes | QtWidgets.QMessageBox.No)
                if save == QtWidgets.QMessageBox.Yes:
                    path, _ = QtWidgets.QFileDialog.getSaveFileName(self, "Save CSV", os.path.join(os.path.expanduser('~'), 'pulsebench_metrics.csv'), "CSV Files (*.csv)")
                    if path:
                        try:
                            with open(path, 'w', newline='') as f:
                                writer = csv.DictWriter(f, fieldnames=['timestamp','cpu_percent','memory_percent','cpu_freq','cpu_temp'])
                                writer.writeheader()
                                for r in self.log_records:
                                    writer.writerow(r)
                            QtWidgets.QMessageBox.information(self, "Saved", f"CSV saved to {path}")
                        except Exception as e:
                            QtWidgets.QMessageBox.critical(self, "Save failed", str(e))

    @QtCore.Slot(str, str)
    def on_process_output(self, stream, line):

        self.log.moveCursor(QtGui.QTextCursor.End)
        self.log.insertPlainText(f"[{stream}] {line}")
     
        if self.log.document().blockCount() > 5000:
            cursor = self.log.textCursor()
            cursor.movePosition(QtGui.QTextCursor.Start)
            cursor.select(QtGui.QTextCursor.LineUnderCursor)
            cursor.removeSelectedText()

    def _check_process(self):
      
        if not self.process:
            return
        if self.process.poll() is not None:
       
            self.process_watcher.stop()
            code = self.process.returncode
            self.status.showMessage(f"Process exited (code={code})")
           
            try:
                if hasattr(self, 'stdout_thread') and self.stdout_thread.isRunning():
                    self.stdout_thread.quit()
                    self.stdout_thread.wait(500)
            except Exception:
                pass
            try:
                if hasattr(self, 'stderr_thread') and self.stderr_thread.isRunning():
                    self.stderr_thread.quit()
                    self.stderr_thread.wait(500)
            except Exception:
                pass
            self.process = None
            self.start_btn.setEnabled(True)
            self.stop_btn.setEnabled(False)
       
            if self.log_records and not self._prompted_save:
                self._prompted_save = True
                save = QtWidgets.QMessageBox.question(self, "Export CSV", "Process ended — export logged metrics to CSV?", QtWidgets.QMessageBox.Yes | QtWidgets.QMessageBox.No)
                if save == QtWidgets.QMessageBox.Yes:
                    path, _ = QtWidgets.QFileDialog.getSaveFileName(self, "Save CSV", os.path.join(os.path.expanduser('~'), 'pulsebench_metrics.csv'), "CSV Files (*.csv)")
                    if path:
                        try:
                            with open(path, 'w', newline='') as f:
                                writer = csv.DictWriter(f, fieldnames=['timestamp','cpu_percent','memory_percent','cpu_freq','cpu_temp'])
                                writer.writeheader()
                                for r in self.log_records:
                                    writer.writerow(r)
                            QtWidgets.QMessageBox.information(self, "Saved", f"CSV saved to {path}")
                        except Exception as e:
                            QtWidgets.QMessageBox.critical(self, "Save failed", str(e))

def main():
    app = QtWidgets.QApplication(sys.argv)
    mw = MainWindow()
    mw.show()
    sys.exit(app.exec())

if __name__ == "__main__":
    main()
