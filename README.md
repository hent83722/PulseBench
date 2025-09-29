# PulseBench

PulseBench is a **cross-platform CPU benchmark** that uses all CPU cores to stress-test your processor and give a performance score based on iterations per second. The longer you run it, the more accurate the results.  

It works on **Linux, Termux (Android), macOS, and Windows**.

---

## Features

- Multi-threaded, uses **100% CPU** across all cores.
- Dynamic scoring based on iterations per second.
- Duration-based benchmarking with **progress bar**.
- Simple, portable, and cross-platform.

---

## Requirements

- **C++17 compatible compiler**: `g++`, `clang++`, or MSVC.
- **CMake** and `make` (for Unix-like platforms).  
- On Windows, `build.bat` uses your installed compiler (MSVC or MinGW).

---

## Build Instructions

### **Linux / macOS / Termux**

1. Open a terminal in the project directory.  
2. Make the build script executable:

#### **Linux**

```bash
chmod +x build.sh
```
3. Run the build Script

```bash
./build.sh
```
- This will create the build folder, compile the project, and generate the 'benchmark' file.

#### **Windows**

1. Open Command Prompt or PowerShell in the project directory.

2. Run the build script:

```cmd
build.bat
```

- This will compile and generate the 'benchmark' file.
- Make sure your compiler (MSVC or MinGW) is in the PATH.

## Running the Benchmark

### Duration-based (recommended, uses all CPU cores)

- In the project directory build folder, run this:

```bash
# Linux / macOS / Termux
./pulsebench --duration 300   # runs for 5 minutes
./pulsebench --duration 60    # runs for 1 minute

# Windows
./ --duration 300
```
- The longer the duration, the more accurate the score.

## Notes

- On Termux, install the following packages before building:

```bash
pkg install clang cmake make
```
- On macOS, install Xcode Command Line Tools:
```bash
xcode-select --install
```

- The score is calculated dynamically based on CPU iterations per second. Faster CPUs get higher scores automatically.

- Use Ctrl+C to stop early, but the full duration is recommended for consistent results.

## Example Output

```diff
Running CPU benchmark for 60 seconds...
[=================>                  ] 50%
...
===== Benchmark Complete =====
CPU Threads: 8
Total Time: 60s
Total Iterations (score): 12345678
```

## Quick Start Table

| Platform           | Build Command | Run Command                      |
| ------------------ | ------------- | -------------------------------- |
| Linux              | `./build.sh`  | `./benchmark --duration 60`     |
| macOS              | `./build.sh`  | `./benchmark --duration 60`     |
| Termux             | `./build.sh`  | `./benchmark --duration 60`     |
| Windows CMD        | `build.bat`   | `./benchmark --duration 60`     |
| Windows PowerShell | `build.bat`   | `./benchmark --duration 60`     |

## License 
PulseBench is released under the MIT License. See LICENSE for details.
