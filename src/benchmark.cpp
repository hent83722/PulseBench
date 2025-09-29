#include "benchmark.hpp"
#include "utils.hpp"
#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <cmath>

std::atomic<bool> stop_flag{false};
std::atomic<uint64_t> total_iterations{0};

// CPU-intensive workload per thread
void cpu_work_thread() {
    const int iterations_per_loop = 1000000;
    while (!stop_flag.load(std::memory_order_relaxed)) {
        double x = 0.0;
        for (int i = 0; i < iterations_per_loop; ++i) {
            x += std::sin(i) * std::tan(i);
        }
        (void)x;  // prevent optimization
        total_iterations.fetch_add(1, std::memory_order_relaxed);
    }
}

// Duration-based benchmark with auto-scaling score
BenchmarkResult run_duration_benchmark(const std::string& /*unused*/, double duration_seconds) {
    BenchmarkResult result{0, 0.0, 0};
    stop_flag = false;
    total_iterations = 0;

    int thread_count = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;

    for (int t = 0; t < thread_count; ++t)
        threads.emplace_back(cpu_work_thread);

    auto start = std::chrono::steady_clock::now();
    const int bar_width = 40;

    while (true) {
        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(now - start).count();
        double progress = elapsed / duration_seconds;
        if (progress > 1.0) progress = 1.0;

        // progress bar
        int pos = static_cast<int>(bar_width * progress);
        std::cout << "\r[";
        for (int i = 0; i < bar_width; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << int(progress * 100.0) << "%";
        std::cout.flush();

        if (elapsed >= duration_seconds)
            break;

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    stop_flag = true;
    for (auto& t : threads) t.join();

    result.total_runs = thread_count;
    result.total_time = duration_seconds;

    // compute dynamic score: iterations per second * multiplier
    double iterations_per_second = total_iterations.load() / duration_seconds;
    result.score = static_cast<uint64_t>(iterations_per_second * 1000); // multiplier for readability

    std::cout << std::endl;
    return result;
}
