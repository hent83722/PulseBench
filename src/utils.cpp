#include "utils.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <numeric>

double mean(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

double median(std::vector<double> values) {
    if (values.empty()) return 0.0;
    std::sort(values.begin(), values.end());
    size_t mid = values.size() / 2;
    if (values.size() % 2 == 0) {
        return (values[mid - 1] + values[mid]) / 2.0;
    } else {
        return values[mid];
    }
}

double stddev(const std::vector<double>& values, double mean_val) {
    if (values.empty()) return 0.0;
    double sum_sq = 0.0;
    for (auto v : values) {
        sum_sq += (v - mean_val) * (v - mean_val);
    }
    return std::sqrt(sum_sq / values.size());
}

void print_results(const std::vector<double>& times) {
    double m = mean(times);
    double med = median(times);
    double sd = stddev(times, m);

    std::cout << "\n=== Benchmark Results ===\n";
    std::cout << "Runs: " << times.size() << "\n";
    std::cout << "Mean: " << m << "s\n";
    std::cout << "Median: " << med << "s\n";
    std::cout << "Std Dev: " << sd << "s\n";
}

void print_progress(double progress) {
    int barWidth = 50;
    std::cout << "[";
    int pos = static_cast<int>(barWidth * progress);
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
}
