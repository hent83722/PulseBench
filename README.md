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

# How PulseBench Works

PulseBench is a cross-platform CPU benchmarking tool designed to measure the raw computational power of your processor. Instead of running arbitrary commands or depending on external programs, it generates a high and consistent CPU workload directly in C++. This ensures results are **portable, reproducible, and fair** across Linux, macOS, Windows, and Termux (Android).

---

## 1. Core Principle

At its heart, PulseBench works by repeatedly executing **mathematical operations** as quickly as possible, across all available CPU cores, for a set duration. The total number of iterations completed in this time is then translated into a **benchmark score**.

Why math operations?  
- They are **CPU-bound** (no reliance on disk, GPU, or memory speeds).  
- They scale well across multiple cores.  
- They are predictable and consistent between runs.  

This makes the score representative of your CPU’s ability to handle heavy computational workloads.

---

## 2. Multi-threaded Execution

When you start PulseBench, it detects the **number of logical CPU threads** available using `std::thread::hardware_concurrency()`.  

- If you have 8 threads, PulseBench spawns 8 worker threads.  
- Each thread runs the same benchmark routine independently.  
- The total work done is aggregated at the end for scoring.  

This ensures that **all cores are fully utilized**, pushing your CPU to 100% usage during the benchmark.

---

## 3. The Benchmark Loop

Each worker thread performs a **tight loop of calculations** (such as repeated floating-point math or prime number checks). These operations are chosen because:

- They are lightweight enough to avoid bottlenecks outside the CPU.  
- They cannot be trivially optimized away by the compiler (ensuring actual work happens).  
- They scale linearly, making them easy to measure.  

The loop continues until the specified **duration** has passed (default is 5 minutes, but you can adjust with `--duration`).  

---

## 4. Time Measurement

PulseBench uses C++’s high-resolution timers (`std::chrono::high_resolution_clock`) to measure:

- **Start time** (when benchmark begins).  
- **End time** (when the chosen duration expires).  
- **Elapsed time** (used to compute how many iterations fit into the given period).  

This guarantees accurate timing across platforms.

---

## 5. Progress Feedback

Unlike traditional benchmarks that leave you waiting silently, PulseBench shows a **live progress bar** in the terminal:

- The bar updates as time passes.  
- It gives an estimate of how much time is left until the benchmark completes.  
- This makes long runs (e.g., 5 minutes) easier to monitor.

---

## 6. Scoring System

After the benchmark duration ends:

1. Each thread reports how many iterations it completed.  
2. The values are summed across all threads.  
3. The total is divided by the elapsed time to get **iterations per second**.  
4. This is then converted into a **PulseBench Score**.  

The longer you run the benchmark, the **more accurate the score** becomes, since small timing variations and background system noise are averaged out.

---

## 7. Portability

PulseBench is written entirely in **C++17** and uses only the **C++ Standard Library**:

- No external dependencies.  
- No reliance on OS-specific APIs.  
- Single codebase works across Linux, macOS, Windows, and Termux.  

This makes it extremely portable and easy to build anywhere a C++ compiler is available.

---

## 8. Efficiency and Use Cases

- **Efficiency**: By default, PulseBench is designed to do the most CPU-intensive thing possible while keeping memory and I/O usage minimal.  
- **Use Cases**:  
  - Comparing CPUs across devices.  
  - Checking scaling behavior of multi-core systems.  
  - Stress-testing processors to ensure stability.  
  - Quick, lightweight benchmark for developers or hobbyists.  

---

## 9. Why Run Longer?

The benchmark is **time-based, not iteration-based**. That means:

- A short 10-second run gives you a rough estimate.  
- A longer 5-minute run averages out more system noise, giving a **stable and repeatable score**.  
- Advanced users may choose even longer runs for precision testing.

---

## 10. Summary

PulseBench works by:

1. Detecting your CPU’s thread count.  
2. Launching worker threads to run intense math calculations.  
3. Measuring how much work can be done within the chosen time.  
4. Aggregating results into a performance score.  
5. Presenting results in a clear, user-friendly way.  

This simple but effective method provides a reliable measure of CPU performance across any platform.

---

📊 **In short**: PulseBench pushes your CPU to its limits, counts how much work it can do, and turns that into a score you can compare with others.


## License 
PulseBench is released under the MIT License. See LICENSE for details.
