#include "search/alns/operators/Destroy/DelayRemoval.hpp"
#include "search/alns/FastRouteCheck.hpp"
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>

namespace soict::alns {
std::vector<std::size_t> DelayRemoval::apply(SearchState& state, std::size_t count,
                                             std::mt19937_64& rng) const {
    const auto& in = state.instance();
    std::vector<std::pair<double, std::size_t>> candidates;
    
    for (std::size_t r = 0; r < state.request_count(); ++r) {
        if (state.location(r).taxi < 0) continue;
        const auto taxi = static_cast<std::size_t>(state.location(r).taxi);
        const auto& route = state.cache(taxi);
        double old_time = route.departure().empty() ? 0 : route.departure().back();
        
        FastRouteCheck newR(in, taxi, route);
        newR.range(0, state.location(r).pickup);
        newR.range(state.location(r).pickup + 1, state.location(r).dropoff);
        newR.range(state.location(r).dropoff + 1, route.nodes().size());
        
        double new_time = newR.departure().value_or(0);
        double delay = old_time - new_time;
        candidates.push_back({delay, r});
    }
    
    std::vector<std::size_t> removed;
    if (candidates.empty()) return removed;
    
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    const double p = 6.0; // Strong determinism for delay removal
    
    while (removed.size() < count && !candidates.empty()) {
        const std::size_t idx = std::min(
            static_cast<std::size_t>(std::pow(dist(rng), p) * candidates.size()),
            candidates.size() - 1);
            
        std::nth_element(candidates.begin(), candidates.begin() + idx,
                         candidates.end(), std::greater<>());
                         
        const auto chosen = candidates[idx].second;
        if (state.remove(chosen)) {
            removed.push_back(chosen);
        }
        candidates.erase(candidates.begin() + idx);
    }
    return removed;
}
} // namespace soict::alns
