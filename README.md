# PulseBench

  
 
**PulseBench** is a high-performance, multi-threaded CPU benchmarking tool designed to measure CPU performance across multiple workload types and provide detailed profiling data. Unlike simple synthetic benchmarks, PulseBench uses a variety of workloads and can report hardware-level counters on Linux for in-depth analysis.

---
   
## PulseBench V1.1.0 is here!

See the release log: [V1.1.0Log.md](./V1.1.0Log.md)

---

## Table of Contents

1. [Features](#features)  
2. [Supported Workloads](#supported-workloads)  
3. [Installation](#installation)  
4. [Building from Source](#building-from-source)  
5. [Usage](#usage)  
6. [Benchmark Output](#benchmark-output)  
7. [Extending PulseBench](#extending-pulsebench)  
8. [Performance Counters](#performance-counters)  
9. [Contributing](#contributing)  
10. [License](#license)  

---

## Features

- Multi-threaded CPU benchmarking.  
- Multiple synthetic and realistic workload types: arithmetic, memory streaming, pointer chasing, branch-heavy, and SIMD operations.  
- Optional hardware-level profiling using Linux `perf_event_open`.  
- Aggregated and per-thread performance reporting.  
- Portable across Linux, with stubbed perf counters on non-Linux platforms.  
- Configurable runtime duration.  
- Modular C++17 codebase with workload registry for easy extension.  

---

## Supported Workloads

PulseBench includes several workloads, each targeting different aspects of CPU performance:

| Workload | Description |
|----------|-------------|
| `compute` | CPU-bound floating-point arithmetic loops. Measures raw computational throughput. |
| `stream` | Sequential memory reads/writes to test memory bandwidth and caching. |
| `pointer_chase` | Random-access pointer chasing to stress memory latency and branch prediction. |
| `branch` | Heavy branching operations to test CPU branch prediction and pipeline efficiency. |
| `simd` | Vectorized SIMD operations using AVX/AVX2 to test modern vector instruction throughput. |

> Workloads are modular and can be extended via the `workload_registry`.

---

## Installation

### Dependencies

- C++17 compatible compiler (GCC 8+, Clang 7+, or MSVC 2019+).  
- CMake 3.10 or higher.  
- Linux only: optional `perf_event` support (`<linux/perf_event.h>`).  
- Threading library (`<thread>`).

---

## Building from Source

Clone the repository and build using CMake:

```bash
git clone https://github.com/hent83722/PulseBench.git
cd PulseBench
mkdir build && cd build
cmake ..
make
```
The resulting executable will be pulsebench in the build directory.

## Usage 

PulseBench is run using the --duration flag to specify how long the benchmark should execute.

```bash
./pulsebench --duration <seconds>
```

Example:

```bash
./pulsebench --duration 10
```
This runs the default benchmark workload for 10 seconds across the number of threads detected on your CPU.

### Notes

- The benchmark runs a predefined set of workloads sequentially.
- The --duration flag specifies total benchmark runtime in seconds.
- Thread count is determined automatically from your system unless modified in the source code.
- Without specifying a workload, PulseBench will run the 'compute' workload by default.

## Benchmark Output Example

After execution, PulseBench produces a report:
```yaml
Running CPU benchmark for 10 seconds...
[=======================================>] 99%
===== Benchmark Complete =====
CPU Threads: 8
Total Time: 10s
Score: 1234567890
```

- Ops: Total operations performed by the thread.

- Ops/sec: Total operations per second across all threads.

- Performance counters (Linux only):

- instructions — total CPU instructions executed

- cycles — total CPU cycles

- cache_refs — total cache references

- cache_misses — total cache misses

- branch_misses — total branch prediction misses

On non-Linux platforms, perf counters are stubbed and will return zero.

## Extending PulseBench

To add a new workload:

1. Create a class inheriting from Workload in workloads.cpp.

2. Implement the required methods:

- init(int thread_id, size_t workset_bytes, size_t seed)

- uint64_t run_batch(size_t batch_size)

- shutdown()

- std::string name() const

3. Register your workload in register_all_workloads():
```cpp
WorkloadRegistry::instance().register_factory("my_workload", [](){ return std::make_unique<MyWorkload>(); });
```

## Performance Counters

Linux-only feature leveraging perf_event_open to measure:

- CPU instructions executed

- CPU cycles

- Cache references and misses

- Branch misses

## Contributing

Contributions are welcome! You can:

- Add new workloads.

- Improve existing workload performance.

- Extend perf counter support.

- Add CSV/JSON export for automated analysis.

## License

PulseBench includes a LICENSE file. See LICENSE for full terms.
