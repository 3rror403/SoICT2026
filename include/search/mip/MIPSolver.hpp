#pragma once

#include "model/Solution.hpp"

namespace soict {

struct MIPResult {
    bool has_solution = false;
    bool optimal = false;
    double benefit = 0;
    double best_bound = 0;
    double seconds = 0;
    Solution solution;
};

MIPResult solve_mip(const Instance& instance, double time_limit_seconds = 300);

} // namespace soict
