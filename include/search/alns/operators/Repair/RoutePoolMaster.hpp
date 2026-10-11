#pragma once
#include "search/alns/operators/Repair/InsertionSearch.hpp"
#include <vector>

namespace soict::alns {
struct MasterColumn {
    std::size_t taxi;
    std::vector<std::size_t> requests;
    double profit;
};
struct MasterResult {
    std::vector<int> selected;
    double profit = 0, root_upper = 0;
    std::size_t nodes = 0, pivots = 0;
    bool timed_out = false;
};
// Restricted set-packing master. The input incumbent must be feasible.
MasterResult solve_route_master(const std::vector<MasterColumn>& columns,
    std::size_t taxis, std::size_t requests, const std::vector<int>& incumbent,
    Deadline deadline);
}
