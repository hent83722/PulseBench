#include "workload_registry.hpp"

WorkloadRegistry& WorkloadRegistry::instance() {
    static WorkloadRegistry instance;
    return instance;
}

void WorkloadRegistry::register_factory(const std::string& name, WorkloadFactory f) {
    factories_[name] = std::move(f);
}

std::unique_ptr<Workload> WorkloadRegistry::create(const std::string& name) const {
    auto it = factories_.find(name);
    if (it == factories_.end()) return nullptr;
    return it->second();
}

std::vector<std::string> WorkloadRegistry::list() const {
    std::vector<std::string> out;
    out.reserve(factories_.size());
    for (auto &p : factories_) out.push_back(p.first);
    return out;
}
