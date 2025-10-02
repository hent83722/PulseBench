#include "workload.hpp"
#include "workload_registry.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <iomanip>
#include <cstdlib>
#include <atomic> // <-- Added this

int main(int argc, char **argv) {
    register_builtin_workloads();

    int duration_seconds = 10;
    if (argc >= 3 && std::string(argv[1]) == "--duration") {
        duration_seconds = std::atoi(argv[2]);
    }

    auto &reg = WorkloadRegistry::instance();
    auto workload = reg.create("simd");
    if (!workload) {
        std::cerr << "Failed to create workload!" << std::endl;
        return 1;
    }

    int threads = std::thread::hardware_concurrency();
    size_t workset_bytes = 128 * 1024 * 1024; // 128 MB per thread
    workload->init(threads, workset_bytes);

    std::cout << "Running CPU benchmark for " << duration_seconds << " seconds..." << std::endl;

    auto start_time = std::chrono::steady_clock::now();
    auto end_time = start_time + std::chrono::seconds(duration_seconds);
    std::atomic<uint64_t> total_score{0};

    std::vector<std::thread> thread_pool;
    for (int t = 0; t < threads; ++t) {
        thread_pool.emplace_back([&]() {
            while (std::chrono::steady_clock::now() < end_time) {
                total_score += workload->run_batch();
            }
        });
    }

    // Progress bar
    while (std::chrono::steady_clock::now() < end_time) {
        auto now = std::chrono::steady_clock::now();
        double fraction = std::chrono::duration<double>(now - start_time).count() / duration_seconds;
        int bar_width = 40;
        int pos = static_cast<int>(bar_width * fraction);
        std::cout << "[";
        for (int i = 0; i < bar_width; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << int(fraction * 100.0) << "%\r";
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    for (auto &th : thread_pool) th.join();

    std::cout << std::endl << "===== Benchmark Complete =====" << std::endl;
    workload->shutdown();

    std::cout << "CPU Threads: " << threads << std::endl;
    std::cout << "Total Time: " << duration_seconds << "s" << std::endl;
    std::cout << "Score: " << total_score.load() << std::endl;

    return 0;
}
