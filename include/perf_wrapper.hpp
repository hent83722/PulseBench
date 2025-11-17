#pragma once
#include <string>
#include <vector>
#include <cstdint>

#ifdef __linux__
#include <linux/perf_event.h>
#endif

struct PerfCounters {
    uint64_t instructions = 0;
    uint64_t cycles = 0;
    uint64_t cache_references = 0;
    uint64_t cache_misses = 0;
    uint64_t branch_misses = 0;
};


struct PerfHandle;

PerfHandle* perf_create();
void perf_reset(PerfHandle* h);
void perf_enable(PerfHandle* h);
void perf_disable(PerfHandle* h);
PerfCounters perf_read(PerfHandle* h);
void perf_destroy(PerfHandle* h);


#ifdef __linux__
namespace PerfEvents {
    constexpr uint32_t TYPE_HW = PERF_TYPE_HARDWARE;
    constexpr uint64_t INSTRUCTIONS = PERF_COUNT_HW_INSTRUCTIONS;
    constexpr uint64_t CYCLES = PERF_COUNT_HW_CPU_CYCLES;
    constexpr uint64_t CACHE_REFS = PERF_COUNT_HW_CACHE_REFERENCES;
    constexpr uint64_t CACHE_MISSES = PERF_COUNT_HW_CACHE_MISSES;
    constexpr uint64_t BRANCH_MISSES = PERF_COUNT_HW_BRANCH_MISSES;
}
#else
namespace PerfEvents {
    constexpr uint32_t TYPE_HW = 0;
    constexpr uint64_t INSTRUCTIONS = 0;
    constexpr uint64_t CYCLES = 0;
    constexpr uint64_t CACHE_REFS = 0;
    constexpr uint64_t CACHE_MISSES = 0;
    constexpr uint64_t BRANCH_MISSES = 0;
}
#endif
