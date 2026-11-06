#include "search/alns/operators/LocalSearch/PassengerRelocate.hpp"
#include "search/alns/operators/Repair/InsertionSearch.hpp"
#include "search/alns/MoveCost.hpp"
#include "search/alns/operators/LocalSearch/RelocationEngine.hpp"

#include <algorithm>
#include <stdexcept>

namespace soict::alns {
std::optional<double> PassengerRelocationContext::calculate_delta_fast(std::size_t taxi,
    const RouteCache& route, std::size_t boundary) const {
    if (!valid() || taxi >= state_.taxi_count() ||
        boundary > route.nodes().size() - (taxi == source_ ? 2 : 0) ||
        (taxi != source_ && !removal_gain_)) return std::nullopt;
    const RouteView view{route.nodes(), pickup_at_, pickup_at_ + 1, taxi == source_};
    const auto delta = adjacent_insertion_cost(state_.instance(), taxi, view, boundary,
        state_.pickup_node(request_), state_.instance().passenger_dropoff(request_));
    return removal_savings_ - delta;
}

std::optional<double> PassengerRelocationContext::gain(std::size_t taxi,
    const RouteCache& route, std::size_t boundary) const {
    if (!valid() || taxi >= state_.taxi_count() ||
        boundary > route.nodes().size() - (taxi == source_ ? 2 : 0))
        return std::nullopt;
    const auto& in = state_.instance();
    const bool same = taxi == source_;
    const auto route_length = route.nodes().size() - (same ? 2 : 0);
    if (same) {
        if (boundary && !route.boundary_allowed(in, original_position(boundary - 1) + 1))
            return std::nullopt;
    } else if (!removal_gain_ || !route.boundary_allowed(in, boundary)) return std::nullopt;

    FastRouteCheck candidate(in, taxi, route);
    if (same) add_source_range(candidate, 0, boundary);
    else candidate.range(0, boundary);
    candidate.node(state_.pickup_node(request_));
    candidate.node(in.passenger_dropoff(request_));
    if (same) add_source_range(candidate, boundary, route_length);
    else candidate.range(boundary, route_length);
    if (!candidate.feasible()) return std::nullopt;
    return calculate_delta_fast(taxi, route, boundary);
}

std::optional<InsertionChoice> PassengerRelocate::best_on_taxi(
    const SearchState& state, const PassengerRelocationContext& context,
    std::size_t r, std::size_t taxi, const RouteCache& route,
    std::uint64_t source_version) const {
    const auto source = static_cast<std::size_t>(state.location(r).taxi);
    if (taxi != source && !context.removal_feasible()) return std::nullopt;
    return cache_.get(r, taxi, state.taxi_count(),
        source_version, route.version(),
        context.removal_savings(), taxi == source, [&] {
                std::optional<InsertionChoice> route_best;
                const auto length = route.nodes().size() - (taxi == source ? 2 : 0);
                for (std::size_t a = 0; a <= length; ++a) {
                    const auto objective = context.calculate_delta_fast(taxi, route, a);
                    if (!objective || *objective + 1e-8 <
                        std::max(1e-9, route_best ? route_best->gain : 1e-9)) continue;
                    const auto gain = context.gain(taxi, route, a);
                    if (gain && (!route_best || *gain > route_best->gain))
                        route_best = InsertionChoice{r, taxi, a, a, *gain};
                }
                return route_best;
            });
}

bool PassengerRelocate::improve(SearchState& state) const {
    return improve_relocations<PassengerRelocationContext, true>(state, 0, state.passenger_count(),
        [this](const SearchState& current, const PassengerRelocationContext& context,
               std::size_t request, std::size_t taxi, const RouteCache& route,
               std::uint64_t source_version) {
            return best_on_taxi(current, context, request, taxi, route, source_version);
        });
}

} // namespace soict::alns
