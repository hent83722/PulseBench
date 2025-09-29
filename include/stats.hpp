#pragma once
#include <vector>

double mean(const std::vector<double>& times);
double median(std::vector<double> times);
double stddev(const std::vector<double>& times, double mean);
