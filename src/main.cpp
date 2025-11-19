#include "workload.hpp"
#include "workload_registry.hpp"
#include "stats.hpp"
#include <nlohmann/json.hpp>
#include <cmath>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <iomanip>
#include <cstdlib>
#include <atomic>
#include <mutex>
#include <fstream>
#include <sstream>
#include <map>

int main(int argc, char **argv) {
    register_builtin_workloads();


    int duration_seconds = 10;
    int threads = std::thread::hardware_concurrency();
    size_t workset_bytes = 128 * 1024 * 1024;
    std::string workload_name = "simd";
    std::string out_file;
    std::string out_format = "json"; 

  
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--duration" && i + 1 < argc) { duration_seconds = std::atoi(argv[++i]); }
        else if (a == "--threads" && i + 1 < argc) { threads = std::atoi(argv[++i]); }
        else if (a == "--workset" && i + 1 < argc) { workset_bytes = std::stoull(argv[++i]); }
        else if (a == "--workload" && i + 1 < argc) { workload_name = argv[++i]; }
        else if (a == "--output" && i + 1 < argc) { out_file = argv[++i]; }
        else if (a == "--format" && i + 1 < argc) { out_format = argv[++i]; }
        else if (a == "--list") {
            auto &r = WorkloadRegistry::instance();
            auto names = r.list();
            std::cout << "Available workloads:\n";
            for (auto &n : names) std::cout << " - " << n << "\n";
            return 0;
        }
        else if (a == "--config" && i + 1 < argc) {
            std::string cfg = argv[++i];
            try {
                std::ifstream ifs(cfg);
                if (!ifs) {
                    std::cerr << "Failed to open config file: " << cfg << std::endl;
                    return 1;
                }
                nlohmann::json j;
                ifs >> j;
                if (j.contains("duration")) duration_seconds = j["duration"].get<int>();
                if (j.contains("threads")) threads = j["threads"].get<int>();
                if (j.contains("workset_bytes")) workset_bytes = j["workset_bytes"].get<size_t>();
                if (j.contains("workload")) workload_name = j["workload"].get<std::string>();
                if (j.contains("output")) out_file = j["output"].get<std::string>();
                if (j.contains("format")) out_format = j["format"].get<std::string>();
            } catch (std::exception &e) {
                std::cerr << "Failed to parse config file: " << e.what() << std::endl;
                return 1;
            }
        }
    }

    auto &reg = WorkloadRegistry::instance();
    auto workload = reg.create(workload_name);
    if (!workload) {
        std::cerr << "Failed to create workload '" << workload_name << "'!" << std::endl;
        std::cerr << "Use --list to see available workloads." << std::endl;
        return 1;
    }

    workload->init(threads, workset_bytes);

    std::cout << "Running benchmark '" << workload_name << "' for " << duration_seconds << " seconds with " << threads << " threads..." << std::endl;

    auto start_time = std::chrono::steady_clock::now();
    auto end_time = start_time + std::chrono::seconds(duration_seconds);
    std::atomic<uint64_t> total_score{0};
    std::atomic<uint64_t> total_batches{0};
    std::vector<double> samples;
    std::mutex samples_mtx;

    std::vector<std::thread> thread_pool;
    for (int t = 0; t < threads; ++t) {
        thread_pool.emplace_back([&]() {
            while (std::chrono::steady_clock::now() < end_time) {
                auto s0 = std::chrono::steady_clock::now();
                uint64_t r = workload->run_batch();
                auto s1 = std::chrono::steady_clock::now();
                double elapsed = std::chrono::duration<double, std::milli>(s1 - s0).count();
                {
                    std::lock_guard<std::mutex> lk(samples_mtx);
                    samples.push_back(elapsed);
                }
                total_score += r;
                total_batches++;
            }
        });
    }


    while (std::chrono::steady_clock::now() < end_time) {
        auto now = std::chrono::steady_clock::now();
        double fraction = std::chrono::duration<double>(now - start_time).count() / duration_seconds;
        int bar_width = 40;
        if (fraction < 0) fraction = 0;
        if (fraction > 1) fraction = 1;
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

    Stats st = compute_stats(samples);


    double throughput = 0.0;
    if (duration_seconds > 0) throughput = static_cast<double>(total_batches.load()) / duration_seconds;


    auto compact_score = [](double t)->int{
        if (t <= 0.0) return 0;
        double v = 1000.0 * std::log10(t + 1.0);
        return static_cast<int>(std::round(v));
    };
    int score = compact_score(throughput);

    std::cout << "Workload: " << workload_name << std::endl;
    std::cout << "CPU Threads: " << threads << std::endl;
    std::cout << "Total Time: " << duration_seconds << "s" << std::endl;
    std::cout << "Total Batches: " << total_batches.load() << std::endl;
    std::cout << "Throughput (batches/s): " << std::fixed << std::setprecision(3) << throughput << std::endl;
    std::cout << "Score: " << score << " (compact)" << std::endl;
    std::cout << "Sample mean (ms): " << st.mean << " median: " << st.median << " stddev: " << st.stddev << "\n";
    std::cout << "min: " << st.min << " max: " << st.max << " p90: " << st.percentiles[90] << " p99: " << st.percentiles[99] << "\n";

    if (!out_file.empty()) {
        std::ofstream ofs(out_file);
        if (!ofs) {
            std::cerr << "Failed to open output file for writing: " << out_file << std::endl;
        } else {
            if (out_format == "json") {

                nlohmann::json jout;
                jout["workload"] = workload_name;
                jout["threads"] = threads;
                jout["duration_seconds"] = duration_seconds;
                jout["total_batches"] = total_batches.load();
                jout["throughput_batches_per_s"] = throughput;
                jout["score"] = score;
                nlohmann::json jstats;
                jstats["mean_ms"] = st.mean;
                jstats["median_ms"] = st.median;
                jstats["stddev_ms"] = st.stddev;
                jstats["min_ms"] = st.min;
                jstats["max_ms"] = st.max;
                nlohmann::json jperc;
                for (auto &p : st.percentiles) jperc[std::to_string(p.first)] = p.second;
                jstats["percentiles"] = jperc;
                jout["stats"] = jstats;

     
                nlohmann::json jhist = nlohmann::json::array();
       
                std::vector<int> hist_bins(10, 0);
                if (!samples.empty()) {
                    double minv = st.min;
                    double maxv = st.max;
                    double range = maxv - minv;
                    if (range <= 0) hist_bins[0] = samples.size();
                    else {
                        for (double v : samples) {
                            int bin = static_cast<int>((v - minv) / range * (hist_bins.size() - 1));
                            if (bin < 0) bin = 0;
                            if (bin >= (int)hist_bins.size()) bin = hist_bins.size() - 1;
                            hist_bins[bin]++;
                        }
                    }
                }
                for (int c : hist_bins) jhist.push_back(c);
                jout["histogram_bins"] = jhist;

                ofs << jout.dump(2) << std::endl;
            } else {

                ofs << "workload,threads,duration_seconds,total_batches,throughput_bps,score,mean_ms,median_ms,stddev_ms,min_ms,max_ms\n";
                ofs << workload_name << "," << threads << "," << duration_seconds << "," << total_batches.load() << "," << throughput << "," << score << "," << st.mean << "," << st.median << "," << st.stddev << "," << st.min << "," << st.max << "\n";
            }
            ofs.close();
            std::cout << "Wrote results to " << out_file << "\n";
        }
    }

    return 0;
}
