#include "benchmark.hpp"
#include "utils.hpp"
#include <iostream>
#include <string>
#include <thread>

// Detect architecture at runtime
#if defined(__x86_64__) || defined(__i386__)
#define IS_X86 1
#else
#define IS_X86 0
#endif

int main(int argc, char** argv) {
    std::cout << "PulseBench ARM Edition\n";
    std::cout << "=======================\n";

    // Block execution if running on x86
#if IS_X86
    std::cout << "❌ This version is for ARM architectures only.\n";
    std::cout << "Please use the x86 edition of PulseBench instead.\n";
    return 1;
#endif

    if (argc > 1) {
        double duration_seconds = std::stod(argv[1]);
        BenchmarkResult res = run_duration_benchmark(duration_seconds);

        std::cout << "CPU Threads: " << res.total_threads << "\n";
        std::cout << "Total Time: " << res.total_time << "s\n";
        std::cout << "Total Iterations: " << res.iterations << "\n";
        std::cout << "Score (scaled): " << res.score << "\n";
    } else {
        int runs = 5;
        std::vector<double> times = run_benchmark(runs);
        print_results(times);
    }

    return 0;
}
