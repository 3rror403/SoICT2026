#include "search/alns/operators/Destroy/RouteRemoval.hpp"
#include <numeric>
#include <algorithm>
#include <random>

namespace soict::alns {

std::vector<std::size_t> RouteRemoval::apply(SearchState& state, std::size_t count,
                                             std::mt19937_64& rng) const {
    std::vector<std::size_t> removed;
    removed.reserve(count);

    // Gather active taxis and their request count
    std::vector<std::pair<std::size_t, std::size_t>> routes;
    for (std::size_t t = 0; t < state.taxi_count(); ++t) {
        // Find how many requests are served by this taxi
        std::size_t served_count = 0;
        for (std::size_t r = 0; r < state.request_count(); ++r) {
            if (state.location(r).taxi == static_cast<int>(t)) {
                ++served_count;
            }
        }
        if (served_count > 0) {
            routes.emplace_back(t, served_count);
        }
    }

    while (removed.size() < count && !routes.empty()) {
        std::vector<double> weights(routes.size());
        for (std::size_t i = 0; i < routes.size(); ++i) {
            weights[i] = 1.0 / static_cast<double>(routes[i].second);
        }

        std::discrete_distribution<std::size_t> dist(weights.begin(), weights.end());
        const std::size_t selected = dist(rng);
        
        const auto taxi = routes[selected].first;
        routes.erase(routes.begin() + selected);

        for (std::size_t r = 0; r < state.request_count(); ++r) {
            if (state.location(r).taxi == static_cast<int>(taxi)) {
                state.remove(r);
                removed.push_back(r);
            }
        }
    }

    return removed;
}

} // namespace soict::alns
