#include "search/alns/operators/LocalSearch/PassengerRelocate.hpp"
#include "search/alns/operators/Repair/InsertionSearch.hpp"
#include "search/alns/MoveCost.hpp"

#include <algorithm>
#include <stdexcept>

namespace soict::alns {
PassengerRelocationContext::PassengerRelocationContext(const SearchState& state,
                                                       std::size_t request)
    : state_(state), request_(request), source_(0), removed_at_(0) {
    if (request >= state.passenger_count() || state.location(request).taxi < 0) return;
    const auto& loc = state.location(request);
    source_ = static_cast<std::size_t>(loc.taxi);
    removed_at_ = loc.pickup;
    served_ = true;
    removal_gain_ = state.removal_gain(request);
    const auto& route = state.cache(source_);
    removal_savings_ = removal_savings_cost(state.instance(), source_,
                                            route.nodes(), loc.pickup, loc.dropoff);
}

std::size_t PassengerRelocationContext::length(std::size_t taxi) const {
    const auto size = state_.cache(taxi).nodes().size();
    return taxi == source_ ? size - 2 : size;
}

std::size_t PassengerRelocationContext::original_position(std::size_t position) const {
    return position < removed_at_ ? position : position + 2;
}

void PassengerRelocationContext::add_source_range(FastRouteCheck& candidate,
                                                  std::size_t first,
                                                  std::size_t end) const {
    const auto left_end = std::min(end, removed_at_);
    if (first < left_end) candidate.range(first, left_end);
    const auto right_start = std::max(first, removed_at_);
    if (right_start < end) candidate.range(right_start + 2, end + 2);
}

std::optional<double> PassengerRelocationContext::gain(std::size_t taxi,
                                                        std::size_t boundary) const {
    if (taxi >= state_.taxi_count()) return std::nullopt;
    return gain(taxi, state_.cache(taxi), boundary);
}

std::optional<double> PassengerRelocationContext::calculate_delta_fast(std::size_t taxi,
    const RouteCache& route, std::size_t boundary) const {
    if (!valid() || taxi >= state_.taxi_count() || boundary > length(taxi) ||
        (taxi != source_ && !removal_gain_)) return std::nullopt;
    const RouteView view{route.nodes(), removed_at_, removed_at_ + 1, taxi == source_};
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
    std::size_t r, std::size_t taxi, const RouteCache& route) const {
    const auto source = static_cast<std::size_t>(state.location(r).taxi);
    if (taxi != source && !context.removal_feasible()) return std::nullopt;
    return cache_.get(r, taxi, state.taxi_count(),
        state.cache(source).version(), route.version(),
        context.removal_savings(), taxi == source, [&] {
                std::optional<InsertionChoice> route_best;
                const auto length = context.length(taxi);
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
    RelocationPool pool;
    const auto scan_request = [&](std::size_t r, bool all,
                                  std::size_t first, std::size_t second) {
        if (state.location(r).taxi < 0) return;
        PassengerRelocationContext context(state, r);
        const auto source = static_cast<std::size_t>(state.location(r).taxi);
        if (!context.valid()) return;
        const auto& index = state.empty_taxi_index();
        const bool indexed = index && index->ensure(r);
        if (indexed) {
            if (all) {
                static const RouteCache empty_route;
                for (const auto& choice : index->choices(r)) {
                    if (state.is_active(choice.taxi)) continue;
                    const auto gain = context.gain(choice.taxi, empty_route, 0);
                    if (gain) pool.add(InsertionChoice{r, choice.taxi, 0, 0, *gain}, source);
                    break;
                }
            }
            for (const auto& [taxi, route] : state.active_routes())
                if (all || taxi == first || taxi == second)
                    pool.add(best_on_taxi(state, context, r, taxi, route), source);
        } else {
            for (std::size_t taxi = 0; taxi < state.taxi_count(); ++taxi)
                if (all || taxi == first || taxi == second)
                    pool.add(best_on_taxi(state, context, r, taxi, state.cache(taxi)), source);
        }
    };
    const auto scan_all = [&] {
        pool.clear();
        for (std::size_t r = 0; r < state.passenger_count(); ++r)
            scan_request(r, true, 0, 0);
    };
    scan_all();
    bool changed = false;
    while (const auto descriptor = pool.best()) {
        const auto choice = descriptor->choice;
        const auto source = descriptor->source;
        const auto target = choice.taxi;
        const bool source_active_before = state.is_active(source);
        const bool target_active_before = state.is_active(target);
        const bool same = source == target;
        const bool applied = same ? state.relocate_within_route(
            choice.request, choice.pickup, choice.delivery) :
            (state.remove(choice.request) && state.insert(choice.request,
                target, choice.pickup, choice.delivery));
        if (!applied)
            throw std::logic_error("passenger relocation descriptor disagrees with mutation");
        changed = true;
        if (state.is_active(source) != source_active_before ||
            state.is_active(target) != target_active_before) {
            scan_all();
            continue;
        }
        pool.erase_affected(source, target);
        for (std::size_t r = 0; r < state.passenger_count(); ++r) {
            if (state.location(r).taxi < 0) continue;
            const auto origin = static_cast<std::size_t>(state.location(r).taxi);
            scan_request(r, origin == source || origin == target, source, target);
        }
    }
    return changed;
}

} // namespace soict::alns
