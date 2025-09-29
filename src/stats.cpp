#include "stats.hpp"
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>

double mean(const std::vector<double>& times) {
    double sum = std::accumulate(times.begin(), times.end(), 0.0);
    return sum / times.size();
}

double median(std::vector<double> times) {
    std::sort(times.begin(), times.end());
    size_t n = times.size();
    if (n % 2 == 0)
        return (times[n/2 - 1] + times[n/2]) / 2.0;
    else
        return times[n/2];
}

double stddev(const std::vector<double>& times, double mean_val) {
    double sq_sum = 0;
    for (auto t : times) sq_sum += (t - mean_val) * (t - mean_val);
    return std::sqrt(sq_sum / times.size());
}
