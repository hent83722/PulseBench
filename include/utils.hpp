#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>

// Stats
double mean(const std::vector<double>& values);
double median(std::vector<double> values);
double stddev(const std::vector<double>& values, double mean);

// Results printing
void print_results(const std::vector<double>& times);

// Progress bar
void print_progress(double progress);

#endif
