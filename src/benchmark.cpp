#include "benchmark.hpp"
#include "utils.hpp"
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <cmath>
#include <iostream>

static std::atomic<uint64_t> total_iterations(0);

void cpu_work_thread(std::atomic<bool>& running) {
    uint64_t local_iterations = 0;
    while (running.load(std::memory_order_relaxed)) {
        int n = (local_iterations % 10000) + 2;
        bool prime = true;
        for (int i = 2; i * i <= n; i++) {
            if (n % i == 0) {
                prime = false;
                break;
            }
        }
        if (prime) {
            // dummy sink
        }
        local_iterations++;
    }
    total_iterations.fetch_add(local_iterations, std::memory_order_relaxed);
}

BenchmarkResult run_duration_benchmark(double duration_seconds) {
    BenchmarkResult result;

    total_iterations.store(0);
    std::atomic<bool> running(true);

    unsigned int thread_count = std::thread::hardware_concurrency();
    if (thread_count == 0) thread_count = 4;
    result.total_threads = thread_count;

    std::vector<std::thread> threads;
    for (unsigned int t = 0; t < thread_count; t++) {
        threads.emplace_back(cpu_work_thread, std::ref(running));
    }

    auto start = std::chrono::high_resolution_clock::now();
    auto end_time = start + std::chrono::duration<double>(duration_seconds);

    while (std::chrono::high_resolution_clock::now() < end_time) {
        auto now = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(now - start).count();
        double progress = elapsed / duration_seconds;
        if (progress > 1.0) progress = 1.0;
        print_progress(progress);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    running.store(false);
    for (auto& th : threads) {
        th.join();
    }

    result.total_time = duration_seconds;
    result.iterations = total_iterations.load(std::memory_order_relaxed);

    double iters_per_sec = static_cast<double>(result.iterations) / result.total_time;
    result.score = static_cast<uint64_t>(iters_per_sec * 1000.0);

    std::cout << "\n"; // move progress bar down
    return result;
}

std::vector<double> run_benchmark(int runs) {
    std::vector<double> times;
    for (int i = 0; i < runs; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        run_duration_benchmark(1.0);
        auto end = std::chrono::high_resolution_clock::now();

        double elapsed = std::chrono::duration<double>(end - start).count();
        times.push_back(elapsed);
    }
    return times;
}
