#include "utils.hpp"
#include "stats.hpp"
#include <chrono>
#include <iostream>
#include <cstdio>
#include <thread>

double run_command(const std::string& cmd) {
    auto start = std::chrono::high_resolution_clock::now();

#ifdef _WIN32
    FILE* pipe = POPEN((cmd + " > NUL 2>&1").c_str(), "r");
#else
    FILE* pipe = POPEN((cmd + " > /dev/null 2>&1").c_str(), "r");
#endif
    if (!pipe) return 0;

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {

    }

    PCLOSE(pipe);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    return duration.count();
}

void print_header() {
    std::cout << "================ Benchmark Results ================\n";
}

void print_results(const std::string& cmd, const std::vector<double>& times) {
    double mn = times.front();
    double mx = times.back();
    double m = mean(times);
    double med = median(times);
    double sd = stddev(times, m);

    std::cout << "\nResults for \"" << cmd << "\":\n";
    std::cout << "Min: " << mn << "s\n";
    std::cout << "Max: " << mx << "s\n";
    std::cout << "Mean: " << m << "s\n";
    std::cout << "Median: " << med << "s\n";
    std::cout << "Stddev: " << sd << "s\n";
}
