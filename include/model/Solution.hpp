#pragma once

#include "model/Instance.hpp"
#include <string>
#include <vector>

namespace soict {

struct Evaluation {
    bool feasible = false;
    double benefit = 0;
    std::string error;
};

// routes[k] lists service-node indices after taxi k's origin.
struct Solution {
    std::vector<std::vector<std::size_t>> routes;

    [[nodiscard]] Evaluation evaluate(const Instance& instance) const;
};

} // namespace soict
