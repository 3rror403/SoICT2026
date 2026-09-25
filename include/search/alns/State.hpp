#pragma once
#include "model/Instance.hpp"
#include "model/Solution.hpp"
#include <vector>

namespace soict {
namespace alns {

class State {
public:
    State(const Instance& instance);
    State(const Instance& instance, const Solution& init_sol);

    // Calculate current benefit/cost
    double benefit() const;
    bool is_feasible() const;

    const Instance& get_instance() const { return instance_; }
    const Solution& get_solution() const { return solution_; }

    // Helpers to insert or remove nodes
    bool insert(int node_id, int taxi_id, int pos);
    bool remove(int node_id);

    // A list of unassigned requests
    std::vector<int> unassigned;

private:
    const Instance& instance_;
    Solution solution_;
    double current_benefit_;
    // Additional tracking data (load, capacities, etc.) can be added here
};

} // namespace alns
} // namespace soict
