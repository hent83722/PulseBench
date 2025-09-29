#ifndef STATS_HPP
#define STATS_HPP

#include <vector>

// Compute mean of a vector
double mean(const std::vector<double>& values);

// Compute median of a vector
double median(std::vector<double> values);

// Compute standard deviation of a vector
double stddev(const std::vector<double>& values, double mean);

#endif // STATS_HPP
