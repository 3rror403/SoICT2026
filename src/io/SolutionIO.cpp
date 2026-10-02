#include "io/SolutionIO.hpp"

#include <cmath>
#include <iomanip>
#include <istream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace soict {
Solution read_solution(std::istream& in, const Instance& instance) {
    instance.validate();
    std::string line;
    double claimed = 0;
    if (!std::getline(in, line)) throw std::runtime_error("missing benefit line");
    std::istringstream benefit_in(line);
    std::string extra;
    if (!(benefit_in >> claimed) || !std::isfinite(claimed) || (benefit_in >> extra))
        throw std::runtime_error("invalid benefit line");
    const auto v = instance.node_count(), k = instance.taxis.size();
    std::vector<std::size_t> internal(v);
    for (std::size_t node = 0; node < v; ++node)
        internal[instance.vertex_id(node) - 1] = node;
    Solution solution;
    solution.routes.resize(k);
    const auto max_length = 2 * (instance.passengers.size() + instance.parcels.size());
    for (auto& route : solution.routes) {
        if (!std::getline(in, line)) throw std::runtime_error("missing taxi route line");
        std::istringstream route_in(line);
        std::size_t length = 0;
        if (!(route_in >> length) || length > max_length)
            throw std::runtime_error("invalid route length");
        route.reserve(length);
        for (std::size_t i = 0; i < length; ++i) {
            std::size_t id = 0;
            if (!(route_in >> id) || id == 0 || id > v || internal[id - 1] < k)
                throw std::runtime_error("invalid or missing service vertex");
            route.push_back(internal[id - 1]);
        }
        if (route_in >> extra) throw std::runtime_error("unexpected data after taxi route");
    }
    if (in >> extra) throw std::runtime_error("unexpected data after solution routes");
    const auto evaluation = solution.evaluate(instance);
    if (!evaluation.feasible) throw std::runtime_error("invalid solution: " + evaluation.error);
    if (std::abs(claimed - evaluation.benefit) > 1e-6 * (1 + std::abs(evaluation.benefit)))
        throw std::runtime_error("claimed benefit differs from recomputed benefit");
    return solution;
}

void write_solution(std::ostream& out, const Solution& solution, const Instance& instance) {
    instance.validate();
    const auto evaluation = solution.evaluate(instance);
    if (!evaluation.feasible) throw std::runtime_error("invalid solution: " + evaluation.error);
    out << std::setprecision(17) << evaluation.benefit << '\n';
    for (const auto& route : solution.routes) {
        out << route.size();
        for (const auto node : route) out << ' ' << instance.vertex_id(node);
        out << '\n';
    }
    if (!out) throw std::runtime_error("failed to write solution");
}
} // namespace soict
