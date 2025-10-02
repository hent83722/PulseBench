#include "workload.hpp"
#include "workload_registry.hpp"
#include <cmath>
#include <memory>
#include <thread>
#include <iostream>

void SIMDWorkload::init(int threads, size_t workset_bytes_) {
    // Fill with some non-zero data
    data.resize(workset_bytes_ / sizeof(float), 0.1f);
    s = 0;
}

// This is the CPU-heavy function
uint64_t SIMDWorkload::run_batch() {
    // Realistic CPU stress: math + memory accesses
    for (size_t i = 0; i < data.size(); ++i) {
        data[i] = std::sin(data[i]) * std::cos(data[i]) + std::sqrt(data[i]);
        s += static_cast<uint64_t>(std::llround(data[i] * 1000));
    }
    return s;
}

void SIMDWorkload::shutdown() {
    data.clear();
}

std::string SIMDWorkload::name() const {
    return "simd";
}

// Register SIMD workload
void register_builtin_workloads() {
    auto &reg = WorkloadRegistry::instance();
    reg.register_factory("simd", []() -> std::unique_ptr<Workload> {
        return std::make_unique<SIMDWorkload>();
    });
}
