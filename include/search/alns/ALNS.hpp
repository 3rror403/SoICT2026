#pragma once
#include "search/alns/Config.hpp"
#include "model/Solution.hpp"
#include <cstddef>

namespace soict::alns {
struct Result {
    Solution solution;
    double benefit = 0;
    double seconds = 0;
    bool timed_out = false;
};
Result solve(const Instance& instance, const Config& config);
} // namespace soict::alns
