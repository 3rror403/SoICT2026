#include "search/alns/operators/Destroy/RouteRemoval.hpp"
#include <algorithm>

namespace soict::alns {
std::vector<std::size_t> RouteRemoval::apply(SearchState& state, std::size_t count,
                                            std::mt19937_64& rng) const {
    auto taxis = state.active_taxis();
    std::shuffle(taxis.begin(), taxis.end(), rng);
    std::vector<std::size_t> removed;
    for (const auto taxi : taxis) {
        if (removed.size() >= count) break;
        std::vector<std::size_t> requests;
        for (std::size_t r = 0; r < state.request_count(); ++r)
            if (state.location(r).taxi == static_cast<int>(taxi)) requests.push_back(r);
        const auto remaining = count - removed.size();
        if (requests.size() <= remaining) {
            if (state.remove_requests(taxi, requests))
                removed.insert(removed.end(), requests.begin(), requests.end());
        } else {
            // Atomic subsets avoid requiring each nonmetric shortcut to be
            // feasible. The returned pool contains only successful removals.
            for (int attempt = 0; attempt < 4; ++attempt) {
                std::shuffle(requests.begin(), requests.end(), rng);
                const std::span<const std::size_t> subset(requests.data(), remaining);
                if (state.remove_requests(taxi, subset)) {
                    removed.insert(removed.end(), subset.begin(), subset.end());
                    break;
                }
            }
        }
    }
    return removed;
}
} // namespace soict::alns
