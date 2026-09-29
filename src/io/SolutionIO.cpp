#include "io/SolutionIO.hpp"

#include <istream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace soict {
Solution read_solution(std::istream& in) {
    std::string magic;
    int version = 0;
    std::size_t taxis = 0;
    if (!(in >> magic >> version >> taxis) || magic != "SOICT2026_SOLUTION" ||
        (version != 1 && version != 2 && version != 3))
        throw std::runtime_error("invalid solution header");
    if (taxis == 0 || taxis > 100000)
        throw std::runtime_error("invalid solution taxi count");
    Solution solution;
    solution.routes.resize(taxis);
    std::size_t active = taxis;
    if (version != 1 && (!(in >> active) || active > taxis))
        throw std::runtime_error("invalid active taxi count");
    if (version == 3) {
        std::string line;
        std::getline(in, line);
        if (line.find_first_not_of(" \t\r") != std::string::npos)
            throw std::runtime_error("unexpected data after active taxi count");
        std::size_t previous_taxi = 0;
        for (std::size_t i = 0; i < active; ++i) {
            if (!std::getline(in, line)) throw std::runtime_error("truncated solution routes");
            std::istringstream route_in(line);
            std::size_t taxi = 0;
            if (!(route_in >> taxi) || taxi >= taxis || (i > 0 && taxi <= previous_taxi))
                throw std::runtime_error("invalid active taxi ID or order");
            previous_taxi = taxi;
            auto& route = solution.routes[taxi];
            std::size_t node = 0;
            while (route_in >> node) {
                route.push_back(node);
                if (route.size() > 40000)
                    throw std::runtime_error("invalid solution route length");
            }
            if (!route_in.eof() || route.empty())
                throw std::runtime_error("invalid solution route");
        }
        std::string extra;
        if (in >> extra) throw std::runtime_error("unexpected data after solution routes");
        return solution;
    }
    std::size_t previous_taxi = 0;
    for (std::size_t i = 0; i < active; ++i) {
        std::size_t taxi = i;
        if (version == 2) {
            if (!(in >> taxi) || taxi >= taxis || (i > 0 && taxi <= previous_taxi))
                throw std::runtime_error("invalid active taxi ID or order");
            previous_taxi = taxi;
        }
        std::size_t length = 0;
        if (!(in >> length) || length > 40000 || (version == 2 && length == 0))
            throw std::runtime_error("invalid solution route length");
        auto& route = solution.routes[taxi];
        route.resize(length);
        for (auto& node : route)
            if (!(in >> node)) throw std::runtime_error("truncated solution route");
    }
    std::string extra;
    if (in >> extra) throw std::runtime_error("unexpected data after solution routes");
    return solution;
}

void write_solution(std::ostream& out, const Solution& solution) {
    std::size_t active = 0;
    for (const auto& route : solution.routes) active += !route.empty();
    out << "SOICT2026_SOLUTION 3\n" << solution.routes.size() << ' ' << active << '\n';
    for (std::size_t taxi = 0; taxi < solution.routes.size(); ++taxi) {
        const auto& route = solution.routes[taxi];
        if (route.empty()) continue;
        out << taxi;
        for (const auto node : route) out << ' ' << node;
        out << '\n';
    }
    if (!out) throw std::runtime_error("failed to write solution");
}
} // namespace soict
