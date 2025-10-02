#pragma once
#include "workload.hpp"
#include <map>
#include <memory>
#include <functional>
#include <string>
#include <vector>

using WorkloadFactory = std::function<std::unique_ptr<Workload>()>;

class WorkloadRegistry {
public:
    static WorkloadRegistry& instance();

    void register_factory(const std::string& name, WorkloadFactory f);
    std::unique_ptr<Workload> create(const std::string& name) const;
    std::vector<std::string> list() const;

private:
    WorkloadRegistry() = default;
    std::map<std::string, WorkloadFactory> factories_;
};
