#include "search/alns/operators/LocalSearch/ParcelRelocate.hpp"
#include "search/alns/operators/Repair/InsertionSearch.hpp"
#include "search/alns/MoveCost.hpp"
#include "search/alns/RequestData.hpp"
#include "search/alns/operators/LocalSearch/RelocationEngine.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>

namespace soict::alns {
ParcelRelocationContext::ParcelRelocationContext(const SearchState& state, std::size_t request)
    : RelocationSource<false>(state, request) {
    if (!valid()) return;
    const auto data = request_data(state.instance(), request);
    pickup_ = single_node(state.instance(), data.pickup);
    delivery_ = single_node(state.instance(), data.delivery);
}

std::optional<double> ParcelRelocationContext::calculate_delta_fast(std::size_t taxi,
    const RouteCache& route, std::size_t a, std::size_t b) const {
    if (!valid() || taxi >= state_.taxi_count() || a > b ||
        b > route.nodes().size() - (taxi == source_ ? 2 : 0) ||
        (taxi != source_ && !removal_gain_)) return std::nullopt;
    const RouteView view{route.nodes(), pickup_at_, delivery_at_, taxi == source_};
    const auto delta = separated_insertion_cost(state_.instance(), taxi, view, a, b,
        state_.pickup_node(request_),
        state_.instance().parcel_dropoff(request_ - state_.passenger_count()));
    return removal_savings_ - delta;
}

std::optional<double> ParcelRelocationContext::gain(std::size_t taxi,
    const RouteCache& route, std::size_t a, std::size_t b) const {
    const auto prefix = pickup_prefix(taxi, route, a);
    return prefix ? gain_from_prefix(taxi, route, a, b, *prefix) : std::nullopt;
}

std::optional<FastRouteCheck> ParcelRelocationContext::pickup_prefix(
    std::size_t taxi, const RouteCache& route, std::size_t a) const {
    if (!valid() || taxi >= state_.taxi_count()) return std::nullopt;
    const bool same = taxi == source_;
    const auto route_length = route.nodes().size() - (same ? 2 : 0);
    if (a > route_length) return std::nullopt;
    const auto& in = state_.instance();
    const auto weight = in.parcels[request_ - state_.passenger_count()].weight;
    if (weight > in.taxis[taxi].parcel_capacity + 1e-8) return std::nullopt;
    if (same) {
        if (!source_boundary_allowed(route, a)) return std::nullopt;
    } else if (!removal_gain_ || !route.boundary_allowed(in, a)) return std::nullopt;

    FastRouteCheck candidate(in, taxi, route);
    if (same) add_source_range(candidate, 0, a);
    else candidate.range(0, a);
    candidate.segment(pickup_);
    // Later delivery positions cannot repair a violation in this fixed prefix.
    if (!candidate.feasible()) return std::nullopt;
    return candidate;
}

std::optional<double> ParcelRelocationContext::gain_from_prefix(
    std::size_t taxi, const RouteCache& route, std::size_t a, std::size_t b,
    const FastRouteCheck& prefix) const {
    const bool same = taxi == source_;
    const auto route_length = route.nodes().size() - (same ? 2 : 0);
    if (a > b || b > route_length) return std::nullopt;
    const auto& in = state_.instance();
    if (same) {
        if (!source_boundary_allowed(route, b)) return std::nullopt;
    } else if (!route.boundary_allowed(in, b)) return std::nullopt;
    auto candidate = prefix;
    if (same) add_source_range(candidate, a, b);
    else candidate.range(a, b);
    candidate.segment(delivery_);
    if (same) add_source_range(candidate, b, route_length);
    else candidate.range(b, route_length);
    if (!candidate.feasible()) return std::nullopt;
    return calculate_delta_fast(taxi, route, a, b);
}

std::optional<InsertionChoice> ParcelRelocate::best_on_taxi(
    const SearchState& state, const ParcelRelocationContext& context,
    std::size_t r, std::size_t taxi, const RouteCache& route,
    std::uint64_t source_version) const {
    const auto source = static_cast<std::size_t>(state.location(r).taxi);
    if (taxi != source && !context.removal_feasible()) return std::nullopt;
    return cache_.get(r, taxi, state.taxi_count(),
        source_version, route.version(),
        context.removal_savings(), taxi == source, [&] {
                std::optional<InsertionChoice> route_best;
                const auto length = route.nodes().size() - (taxi == source ? 2 : 0);
                const auto& in = state.instance();
                const auto loc = state.location(r);
                const RouteView view{route.nodes(), loc.pickup, loc.dropoff,
                                     taxi == source};
                const auto pickup_node = state.pickup_node(r);
                const auto delivery_node = in.parcel_dropoff(r - state.passenger_count());
                std::vector<double> pickup_cost(length + 1), delivery_cost(length + 1),
                                    adjacent_cost(length + 1),
                                    suffix_delivery(length + 2,
                                        std::numeric_limits<double>::infinity());
                for (std::size_t pos = 0; pos <= length; ++pos) {
                    pickup_cost[pos] = single_insertion_cost(in, taxi, view,
                                                             pos, pickup_node);
                    delivery_cost[pos] = single_insertion_cost(in, taxi, view,
                                                               pos, delivery_node);
                    adjacent_cost[pos] = adjacent_insertion_cost(in, taxi, view,
                                             pos, pickup_node, delivery_node);
                }
                for (std::size_t pos = length + 1; pos-- > 0;)
                    suffix_delivery[pos] = std::min(delivery_cost[pos],
                                                    suffix_delivery[pos + 1]);
                for (std::size_t a = 0; a <= length; ++a) {
                    const auto best_delta = std::min(adjacent_cost[a],
                        pickup_cost[a] + suffix_delivery[a + 1]);
                    if (context.removal_savings() - best_delta + 1e-8 <
                        std::max(1e-9, route_best ? route_best->gain : 1e-9)) continue;
                    const auto prefix = context.pickup_prefix(taxi, route, a);
                    if (!prefix) continue;
                    for (std::size_t b = a; b <= length; ++b) {
                        const auto delta = b == a ? adjacent_cost[a] :
                                           pickup_cost[a] + delivery_cost[b];
                        const auto objective = context.removal_savings() - delta;
                        if (objective + 1e-8 <
                            std::max(1e-9, route_best ? route_best->gain : 1e-9)) continue;
                        const auto gain = context.gain_from_prefix(taxi, route, a, b, *prefix);
                        if (gain && (!route_best || *gain > route_best->gain))
                            route_best = InsertionChoice{r, taxi, a, b, *gain};
                    }
                }
                return route_best;
            });
}

bool ParcelRelocate::improve(SearchState& state) const {
    return improve_relocations<ParcelRelocationContext, false>(state, state.passenger_count(), state.request_count(),
        [this](const SearchState& current, const ParcelRelocationContext& context,
               std::size_t request, std::size_t taxi, const RouteCache& route,
               std::uint64_t source_version) {
            return best_on_taxi(current, context, request, taxi, route, source_version);
        });
}

} // namespace soict::alns
