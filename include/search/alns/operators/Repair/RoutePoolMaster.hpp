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
    // Optional full-bank request prices for the next generation phase.
    // These guide route generation; they do not certify a restricted solve.
    std::vector<double> request_prices;
    double profit = 0, root_upper = 0;
    std::size_t nodes = 0, pivots = 0;
    bool timed_out = false;
    bool complete = false;
};
std::vector<double> route_master_prices(const std::vector<MasterColumn>& columns,
    std::size_t taxis, std::size_t requests, Deadline deadline);
bool route_master_uses_cplex();
// Restricted set-packing master. The input incumbent must be feasible.
MasterResult solve_route_master(const std::vector<MasterColumn>& columns,
    std::size_t taxis, std::size_t requests, const std::vector<int>& incumbent,
    Deadline deadline, bool verbose = true);
// Coordinated LP neighborhoods preserve a feasible fleet throughout. This
// heuristic does not claim that the unrestricted bank master is complete.
MasterResult improve_route_master(const std::vector<MasterColumn>& columns,
    std::size_t taxis, std::size_t requests, const std::vector<int>& incumbent,
    Deadline deadline, std::uint64_t seed = 1, const std::vector<double>& request_prices = {});
}
