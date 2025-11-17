#pragma once
#include <string>
#include <vector>

struct BenchmarkResult {
    int total_runs;
    double total_time;
    int score;
};


std::vector<double> run_benchmark(const std::string& cmd, int runs = 5);


BenchmarkResult run_duration_benchmark(const std::string& cmd, double duration_seconds);
