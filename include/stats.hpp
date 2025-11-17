#pragma once
#include <vector>
#include <map>

struct Stats {
	double mean = 0.0;
	double median = 0.0;
	double stddev = 0.0;
	double min = 0.0;
	double max = 0.0;
	std::map<int,double> percentiles;
};

double mean(const std::vector<double>& times);
double median(std::vector<double> times);
double stddev(const std::vector<double>& times, double mean);
double percentile(std::vector<double> times, double p);
Stats compute_stats(const std::vector<double>& times, const std::vector<int>& percentiles_to_compute = {50,90,99});
