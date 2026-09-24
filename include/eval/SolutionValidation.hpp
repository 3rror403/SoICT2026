#pragma once

#include "model/Solution.hpp"

namespace soict {

// Recompute objective and reject any route that violates the contest rules.
double validate_solution(const Instance& instance, const Solution& solution);

} // namespace soict
