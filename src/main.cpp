#include "benchmark.hpp"
#include "utils.hpp"
#include "stats.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <thread>

int main(int argc, char** argv) {
    bool duration_mode = false;
    double duration_seconds = 300.0; // default 5 minutes

    if (argc >= 2 && std::string(argv[1]) == "--duration") {
        duration_mode = true;
        if (argc >= 3) duration_seconds = std::stod(argv[2]);
    }

    if (duration_mode) {
        std::cout << "Running CPU benchmark for " << duration_seconds 
                  << " seconds...\n";
        BenchmarkResult result = run_duration_benchmark("", duration_seconds);
        std::cout << "\n===== Benchmark Complete =====\n";
        std::cout << "CPU Threads: " << std::thread::hardware_concurrency() << "\n";
        std::cout << "Total Time: " << result.total_time << "s\n";
        std::cout << "Score(Total Iterations): " << result.score << "\n";
    } else {
        std::cout << "Use '--duration <seconds>' for duration-based benchmark.\n";
    }

    return 0;
}
