#include "search/alns/operators/Destroy/OpportunityRemoval.hpp"
#include "search/alns/operators/Repair/InsertionSearch.hpp"
#include <algorithm>
#include <cmath>

namespace soict::alns {
std::vector<std::size_t> OpportunityRemoval::apply(SearchState& state, std::size_t count,
                                                   std::mt19937_64& rng) const {
    if (!count) return {};
    const auto& in = state.instance();
    const auto& index = state.empty_taxi_index();
    std::vector<std::size_t> targets, served;
    std::vector<double> weights;
    for (std::size_t r = 0; r < state.request_count(); ++r) {
        if (state.location(r).taxi >= 0) { served.push_back(r); continue; }
        double opportunity = state.revenue(r);
        if (index) {
            index->ensure(r);
            const auto& choices = index->choices(r);
            opportunity = choices.empty() ? 0 : std::max(0.0, choices.front().gain);
        }
        if (opportunity > 1e-9) { targets.push_back(r); weights.push_back(opportunity); }
    }
    if (served.empty()) return {};
    std::size_t anchor;
    if (targets.empty()) anchor = served[rng() % served.size()];
    else anchor = targets[std::discrete_distribution<std::size_t>(weights.begin(), weights.end())(rng)];
    const auto dropoff = [&](std::size_t r) {
        return r < state.passenger_count() ? in.passenger_dropoff(r) :
            in.parcel_dropoff(r - state.passenger_count());
    };
    const auto earliest = [&](std::size_t r) {
        return r < state.passenger_count() ? in.passengers[r].pickup.earliest :
            in.parcels[r - state.passenger_count()].pickup.earliest;
    };
    const auto pickup_a = state.pickup_node(anchor), drop_a = dropoff(anchor);
    std::vector<std::pair<double, std::size_t>> ranked;
    for (auto r : served) {
        const auto pickup = state.pickup_node(r), drop = dropoff(r);
        const double distance = 0.5 * (in.time(pickup_a, pickup) + in.time(pickup, pickup_a) +
            in.time(drop_a, drop) + in.time(drop, drop_a)) + std::abs(earliest(anchor) - earliest(r));
        ranked.emplace_back(distance, r);
    }
    std::sort(ranked.begin(), ranked.end());
    std::uniform_real_distribution<double> uniform(0, 1);
    std::vector<std::size_t> removed;
    while (!ranked.empty() && removed.size() < count) {
        const auto position = std::min(ranked.size() - 1,
            static_cast<std::size_t>(std::pow(uniform(rng), 3) * ranked.size()));
        const auto r = ranked[position].second;
        ranked.erase(ranked.begin() + position);
        if (state.remove(r)) removed.push_back(r);
    }
    return removed;
}
} // namespace soict::alns
