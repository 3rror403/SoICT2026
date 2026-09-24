#include "io/SolutionIO.hpp"

#include <istream>
#include <ostream>
#include <stdexcept>
#include <string>

namespace soict {
Solution read_solution(std::istream& in) {
    std::string magic;
    int version = 0;
    std::size_t taxis = 0;
    if (!(in >> magic >> version >> taxis) || magic != "SOICT2026_SOLUTION" || version != 1)
        throw std::runtime_error("invalid solution header");
    if (taxis == 0 || taxis > 100000)
        throw std::runtime_error("invalid solution taxi count");
    Solution solution;
    solution.routes.resize(taxis);
    for (auto& route : solution.routes) {
        std::size_t length = 0;
        if (!(in >> length) || length > 40000)
            throw std::runtime_error("invalid solution route length");
        route.resize(length);
        for (auto& node : route)
            if (!(in >> node)) throw std::runtime_error("truncated solution route");
    }
    std::string extra;
    if (in >> extra) throw std::runtime_error("unexpected data after solution routes");
    return solution;
}

void write_solution(std::ostream& out, const Solution& solution) {
    out << "SOICT2026_SOLUTION 1\n" << solution.routes.size() << '\n';
    for (const auto& route : solution.routes) {
        out << route.size();
        for (const auto node : route) out << ' ' << node;
        out << '\n';
    }
    if (!out) throw std::runtime_error("failed to write solution");
}
} // namespace soict
