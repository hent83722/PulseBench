#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "stats.hpp"
#include <vector>

TEST_CASE("mean and median and stddev basic") {
    std::vector<double> v = {1.0, 2.0, 3.0, 4.0};
    REQUIRE(mean(v) == Approx(2.5));
    REQUIRE(median(v) == Approx(2.5));
    double sd = stddev(v, mean(v));
    REQUIRE(sd == Approx(1.1180).epsilon(1e-3));
}

TEST_CASE("percentile and compute_stats") {
    std::vector<double> v;
    for (int i = 1; i <= 100; ++i) v.push_back(static_cast<double>(i));
    REQUIRE(percentile(v, 50.0) == Approx(50.0));
    REQUIRE(percentile(v, 90.0) == Approx(90.0));

    Stats s = compute_stats(v, {50,90,99});
    REQUIRE(s.mean == Approx(50.5));
    REQUIRE(s.median == Approx(50.5));
    REQUIRE(s.percentiles.at(50) == Approx(50.0));
    REQUIRE(s.percentiles.at(90) == Approx(90.0));
}
