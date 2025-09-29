#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include <cstdint>
#include <vector>

struct BenchmarkResult {
    uint64_t iterations = 0;
    double total_time = 0.0;
    uint32_t total_threads = 0;
    uint64_t score = 0;
};

// Run for a fixed duration (seconds)
BenchmarkResult run_duration_benchmark(double duration_seconds);

// Run for a fixed number of 1-second runs
std::vector<double> run_benchmark(int runs);

#endif
