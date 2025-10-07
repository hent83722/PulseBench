PulseBench GUI

This is a simple GUI wrapper for the PulseBench CLI binary. It provides:

- Start / Stop button to run the compiled `build/pulsebench` binary
- Live charts for CPU temperature, CPU usage, CPU frequency and memory usage using `pyqtgraph`

Requirements

- Python 3.8+
- Create a virtual environment and install requirements:

```bash
python -m venv .venv
source .venv/bin/activate
pip install -r gui/requirements.txt
```

Usage

- Ensure the CLI binary exists at `build/pulsebench`. If not, build the project (see top-level README).
- Run the GUI:

```bash
python gui/main.py
```

Notes

- The GUI will attempt to spawn `./build/pulsebench` in the repository root. If your binary is elsewhere, update the `BINARY_PATH` constant in `gui/main.py`.
- On systems without temperature sensors available, CPU temperature plot will stay empty.
