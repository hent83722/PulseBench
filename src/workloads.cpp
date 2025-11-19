#include "workload.hpp"
#include "workload_registry.hpp"
#include <cmath>
#include <memory>
#include <thread>
#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>

void SIMDWorkload::init(int threads, size_t workset_bytes_) {

    data.resize(workset_bytes_ / sizeof(float), 0.1f);
    s = 0;
}


uint64_t SIMDWorkload::run_batch() {

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


void register_builtin_workloads() {
    auto &reg = WorkloadRegistry::instance();
    reg.register_factory("simd", []() -> std::unique_ptr<Workload> {
        return std::make_unique<SIMDWorkload>();
    });

    reg.register_factory("memcpy", []() -> std::unique_ptr<Workload> {
        class MemcpyWorkload : public Workload {
        public:
            void init(int threads, size_t workset_bytes) override {
                buf.resize(workset_bytes / 2, 1);
                tmp.resize(workset_bytes / 2);
            }
            uint64_t run_batch() override {
                for (size_t i = 0; i + 1024 < buf.size(); i += 1024) {
                    std::memcpy(&tmp[i], &buf[i], 1024);
                }
                counter++;
                return counter;
            }
            void shutdown() override { buf.clear(); tmp.clear(); }
            std::string name() const override { return "memcpy"; }
        private:
            std::vector<char> buf;
            std::vector<char> tmp;
            uint64_t counter = 0;
        };
        return std::make_unique<MemcpyWorkload>();
    });

    reg.register_factory("io", []() -> std::unique_ptr<Workload> {
        class IOWorkload : public Workload {
        public:
            void init(int threads, size_t workset_bytes) override {

                path = "/tmp/pulsebench_io_test.bin";

                std::ofstream ofs(path, std::ios::binary | std::ios::trunc);
                std::vector<char> block(4096, 'x');
                for (int i=0;i<4;i++) ofs.write(block.data(), block.size());
            }
            uint64_t run_batch() override {
                std::ofstream ofs(path, std::ios::binary | std::ios::app);
                std::vector<char> block(4096, 'y');
                ofs.write(block.data(), block.size());
                ofs.close();
                counter++;
                return counter;
            }
            void shutdown() override { /* leave file for inspection */ }
            std::string name() const override { return "io"; }
        private:
            std::string path;
            uint64_t counter = 0;
        };
        return std::make_unique<IOWorkload>();
    });
}
