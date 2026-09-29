#include "search/alns/operators/Destroy/RelatedRemoval.hpp"

#include <cmath>
#include <limits>
#include <vector>

namespace soict::alns {
std::vector<std::size_t> RelatedRemoval::apply(SearchState& state, std::size_t count,
                                std::mt19937_64& rng) const {
    std::vector<std::size_t> served;
    for (std::size_t r = 0; r < state.request_count(); ++r)
        if (state.location(r).taxi >= 0) served.push_back(r);
    if (served.empty()) return {};
    const auto anchor = served[rng() % served.size()];
    const auto& in = state.instance();
    const auto anchor_node = state.pickup_node(anchor);
    const auto earliest = [&](std::size_t r) {
        return r < in.passengers.size() ? in.passengers[r].pickup.earliest :
                                            in.parcels[r - in.passengers.size()].pickup.earliest;
    };
    const double anchor_time = earliest(anchor);
    // Shaw-style proximity in pickup space and time. Directed travel times
    // are used in both directions; no triangle inequality is assumed.
    std::vector<std::size_t> removed;
    removed.reserve(count);
    while (removed.size() < count) {
        std::size_t choice = state.request_count();
        double best_distance = std::numeric_limits<double>::infinity();
        for (std::size_t r = 0; r < state.request_count(); ++r) {
            if (state.location(r).taxi < 0 || !state.removal_gain(r)) continue;
            const auto node = state.pickup_node(r);
            const double distance = in.time(anchor_node, node) + in.time(node, anchor_node)
                                  + std::abs(anchor_time - earliest(r));
            if (distance < best_distance) { best_distance = distance; choice = r; }
        }
        if (choice == state.request_count() || !state.remove(choice)) break;
        removed.push_back(choice);
    }
    return removed;
}
} // namespace soict::alns
