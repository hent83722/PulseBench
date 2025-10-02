#pragma once
#include <vector>
#include <string>
#include <cstdint>

class Workload {
public:
    virtual ~Workload() = default;
    virtual void init(int threads, size_t workset_bytes) = 0;
    virtual uint64_t run_batch() = 0;
    virtual void shutdown() = 0;
    virtual std::string name() const = 0;
};

class SIMDWorkload : public Workload {
public:
    void init(int threads, size_t workset_bytes) override;
    uint64_t run_batch() override;
    void shutdown() override;
    std::string name() const override;

private:
    std::vector<float> data;
    uint64_t s = 0;
};

void register_builtin_workloads();
