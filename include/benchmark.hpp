#pragma once
#include <string>
#include <vector>

struct BenchmarkResult {
    int total_runs;
    double total_time;
    int score;
};

// Run a command a fixed number of times (traditional benchmarking)
std::vector<double> run_benchmark(const std::string& cmd, int runs = 5);

// Run a command repeatedly for a fixed duration (time-based benchmark)
BenchmarkResult run_duration_benchmark(const std::string& cmd, double duration_seconds);
