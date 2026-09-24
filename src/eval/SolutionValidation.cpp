#include "eval/SolutionValidation.hpp"

#include <stdexcept>

namespace soict {
double validate_solution(const Instance& instance, const Solution& solution) {
    const auto result = solution.evaluate(instance);
    if (!result.feasible) throw std::runtime_error("invalid solution: " + result.error);
    return result.benefit;
}
} // namespace soict
