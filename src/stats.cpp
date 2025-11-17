#include "stats.hpp"
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <limits>

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

double percentile(std::vector<double> times, double p) {
    if (times.empty()) return 0.0;
    std::sort(times.begin(), times.end());
    if (p <= 0) return times.front();
    if (p >= 100) return times.back();
    double idx = (p / 100.0) * (times.size() - 1);
    size_t lo = static_cast<size_t>(std::floor(idx));
    size_t hi = static_cast<size_t>(std::ceil(idx));
    if (lo == hi) return times[lo];
    double frac = idx - lo;
    return times[lo] * (1.0 - frac) + times[hi] * frac;
}

Stats compute_stats(const std::vector<double>& times, const std::vector<int>& percentiles_to_compute) {
    Stats s;
    if (times.empty()) return s;
    s.mean = mean(times);
    s.median = median(times);
    s.stddev = stddev(times, s.mean);
    s.min = *std::min_element(times.begin(), times.end());
    s.max = *std::max_element(times.begin(), times.end());
    for (int p : percentiles_to_compute) {
        s.percentiles[p] = percentile(times, static_cast<double>(p));
    }
    return s;
}
