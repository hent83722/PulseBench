#include "perf_wrapper.hpp"
#include <cstring>
#ifdef __linux__
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>

static long sys_perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                     int cpu, int group_fd, unsigned long flags) {
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

struct PerfHandle {
    int fd_instructions = -1;
    int fd_cycles = -1;
    int fd_cache_refs = -1;
    int fd_cache_misses = -1;
    int fd_branch_misses = -1;
};

static int open_hw_counter(__u32 type, __u64 config) {
    struct perf_event_attr pe;
    memset(&pe, 0, sizeof(pe));
    pe.type = type;
    pe.size = sizeof(pe);
    pe.config = config;
    pe.disabled = 1;
    pe.exclude_kernel = 1;
    pe.exclude_hv = 1;
    int fd = (int)sys_perf_event_open(&pe, 0, -1, -1, 0);
    if (fd == -1) return -1;
    return fd;
}

PerfHandle* perf_create() {
    PerfHandle* h = new PerfHandle();
    h->fd_instructions = open_hw_counter(PERF_TYPE_HARDWARE, PERF_COUNT_HW_INSTRUCTIONS);
    h->fd_cycles = open_hw_counter(PERF_TYPE_HARDWARE, PERF_COUNT_HW_CPU_CYCLES);
    h->fd_cache_refs = open_hw_counter(PERF_TYPE_HW_CACHE, PERF_COUNT_HW_CACHE_REFERENCES);
    h->fd_cache_misses = open_hw_counter(PERF_TYPE_HW_CACHE, PERF_COUNT_HW_CACHE_MISSES);
    h->fd_branch_misses = open_hw_counter(PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_MISSES);

    if (h->fd_instructions == -1 && h->fd_cycles == -1 &&
        h->fd_cache_refs == -1 && h->fd_cache_misses == -1 && h->fd_branch_misses == -1) {
        delete h;
        return nullptr;
    }
    return h;
}

void perf_reset(PerfHandle* h) {
    if (!h) return;
    auto r = [&](int fd){ if (fd!=-1) ioctl(fd, PERF_EVENT_IOC_RESET, 0); };
    r(h->fd_instructions); r(h->fd_cycles); r(h->fd_cache_refs); r(h->fd_cache_misses); r(h->fd_branch_misses);
}
void perf_enable(PerfHandle* h) {
    if (!h) return;
    auto e = [&](int fd){ if (fd!=-1) ioctl(fd, PERF_EVENT_IOC_ENABLE, 0); };
    e(h->fd_instructions); e(h->fd_cycles); e(h->fd_cache_refs); e(h->fd_cache_misses); e(h->fd_branch_misses);
}
void perf_disable(PerfHandle* h) {
    if (!h) return;
    auto d = [&](int fd){ if (fd!=-1) ioctl(fd, PERF_EVENT_IOC_DISABLE, 0); };
    d(h->fd_instructions); d(h->fd_cycles); d(h->fd_cache_refs); d(h->fd_cache_misses); d(h->fd_branch_misses);
}

static uint64_t read_fd_val(int fd) {
    if (fd == -1) return 0;
    uint64_t val = 0;
    ssize_t r = read(fd, &val, sizeof(val));
    if (r != (ssize_t)sizeof(val)) return 0;
    return val;
}

PerfCounters perf_read(PerfHandle* h) {
    PerfCounters p{};
    if (!h) return p;
    p.instructions = read_fd_val(h->fd_instructions);
    p.cycles = read_fd_val(h->fd_cycles);
    p.cache_references = read_fd_val(h->fd_cache_refs);
    p.cache_misses = read_fd_val(h->fd_cache_misses);
    p.branch_misses = read_fd_val(h->fd_branch_misses);
    return p;
}

void perf_destroy(PerfHandle* h) {
    if (!h) return;
    auto c = [&](int &fd){ if (fd!=-1) { close(fd); fd = -1; } };
    c(h->fd_instructions); c(h->fd_cycles); c(h->fd_cache_refs); c(h->fd_cache_misses); c(h->fd_branch_misses);
    delete h;
}

#else
// No-op stubs for non-linux
struct PerfHandle {};
PerfHandle* perf_create(){ return nullptr; }
void perf_reset(PerfHandle*) {}
void perf_enable(PerfHandle*) {}
void perf_disable(PerfHandle*) {}
PerfCounters perf_read(PerfHandle*) { return PerfCounters(); }
void perf_destroy(PerfHandle*) {}
#endif
