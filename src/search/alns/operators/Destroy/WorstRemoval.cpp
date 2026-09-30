#include "search/alns/operators/Destroy/WorstRemoval.hpp"

#include <algorithm>
#include <cmath>
#include <random>
#include <utility>

namespace soict::alns {
std::vector<std::size_t> WorstRemoval::apply(SearchState& state, std::size_t count,
                                std::mt19937_64& rng) const {
    std::vector<std::size_t> removed;
    removed.reserve(count);

    // Phase 3b: Collect all requests and their current removal gains.
    std::vector<std::pair<double, std::size_t>> candidates;
    candidates.reserve(state.request_count());
    for (std::size_t r = 0; r < state.request_count(); ++r) {
        if (const auto gain = state.removal_gain(r)) {
            candidates.emplace_back(*gain, r);
        }
    }

    std::uniform_real_distribution<double> dist(0.0, 1.0);
    const double p = 3.0; // Determinism power
    
    while (removed.size() < count && !candidates.empty()) {
        // Pick randomly using u^p distribution.
        const std::size_t idx = std::min(
            static_cast<std::size_t>(std::pow(dist(rng), p) * candidates.size()),
            candidates.size() - 1);
            
        // Find the element that belongs at idx in a descending sorted array
        std::nth_element(candidates.begin(), candidates.begin() + idx,
                         candidates.end(), std::greater<>());
                         
        const auto chosen = candidates[idx].second;
        const auto taxi = state.location(chosen).taxi;
        
        if (!state.remove(chosen)) {
            candidates.erase(candidates.begin() + idx);
            continue;
        }
        
        removed.push_back(chosen);
        candidates.erase(candidates.begin() + idx);

        // Update gains only for requests on the same route
        for (auto& [gain, r] : candidates) {
            if (state.location(r).taxi == taxi) {
                gain = state.removal_gain(r).value_or(-1e18);
            }
        }
        std::erase_if(candidates, [](const auto& c) { return c.first <= -1e17; });
    }
    return removed;
}
} // namespace soict::alns
