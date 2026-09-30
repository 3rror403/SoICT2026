#include "search/alns/operators/LocalSearch/ParcelRelocate.hpp"
#include "search/alns/operators/Repair/InsertionSearch.hpp"
#include "search/alns/MoveCost.hpp"

#include <algorithm>
#include <cassert>
#include <limits>
#include <optional>
#include <stdexcept>
#include <vector>

namespace soict::alns {
namespace {
constexpr double kMinGain = 1e-9;
constexpr double kTolerance = 1e-8;

struct PairCandidate {
    double objective;
    std::size_t pickup;
    std::size_t delivery;
};

// Phase 1b: thread_local scratch — tránh 4–5 lần malloc/free cho mỗi cặp
// (request, taxi) bị cache-miss. Mỗi thread giữ một bản riêng.
struct ParcelScratch {
    std::vector<double> pickup_cost, delivery_cost, adjacent_cost, suffix_delivery;
    std::vector<PairCandidate> pairs;
};
} // namespace

ParcelRelocationContext::ParcelRelocationContext(const SearchState& state,
                                                 std::size_t request)
    : state_(state), request_(request), source_(0), pickup_at_(0), delivery_at_(0) {
    if (request < state.passenger_count() || request >= state.request_count() ||
        state.location(request).taxi < 0) return;
    const auto& loc = state.location(request);
    source_ = static_cast<std::size_t>(loc.taxi);
    pickup_at_ = loc.pickup;
    delivery_at_ = loc.dropoff;
    served_ = true;
    removal_gain_ = state.removal_gain(request);
    pickup_ = single_node(state.instance(), state.pickup_node(request));
    delivery_ = single_node(state.instance(),
        state.instance().parcel_dropoff(request - state.passenger_count()));
    const auto& route = state.cache(source_);
    removal_savings_ = removal_savings_cost(state.instance(), source_,
                                            route.nodes(), loc.pickup, loc.dropoff);
}

std::size_t ParcelRelocationContext::length(std::size_t taxi) const {
    const auto size = state_.cache(taxi).nodes().size();
    return taxi == source_ ? size - 2 : size;
}

std::size_t ParcelRelocationContext::original_position(std::size_t position) const {
    if (position < pickup_at_) return position;
    if (position < delivery_at_ - 1) return position + 1;
    return position + 2;
}

bool ParcelRelocationContext::source_boundary_allowed(const RouteCache& route,
                                                       std::size_t boundary) const {
    return !boundary || route.boundary_allowed(state_.instance(),
                                               original_position(boundary - 1) + 1);
}

void ParcelRelocationContext::add_source_range(FastRouteCheck& candidate,
                                               std::size_t first,
                                               std::size_t end) const {
    const auto left_end = std::min(end, pickup_at_);
    if (first < left_end) candidate.range(first, left_end);
    const auto middle_start = std::max(first, pickup_at_);
    const auto middle_end = std::min(end, delivery_at_ - 1);
    if (middle_start < middle_end)
        candidate.range(middle_start + 1, middle_end + 1);
    const auto right_start = std::max(first, delivery_at_ - 1);
    if (right_start < end) candidate.range(right_start + 2, end + 2);
}

std::optional<double> ParcelRelocationContext::gain(std::size_t taxi,
    std::size_t a, std::size_t b) const {
    if (taxi >= state_.taxi_count()) return std::nullopt;
    return gain(taxi, state_.cache(taxi), a, b);
}

std::optional<double> ParcelRelocationContext::calculate_delta_fast(std::size_t taxi,
    const RouteCache& route, std::size_t a, std::size_t b) const {
    if (!valid() || taxi >= state_.taxi_count() || a > b || b > length(taxi) ||
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
    std::size_t r, std::size_t taxi, const RouteCache& route) const {
    const auto source = static_cast<std::size_t>(state.location(r).taxi);
    if (taxi != source && !context.removal_feasible()) return std::nullopt;
    return cache_.get(r, taxi, state.taxi_count(),
        state.cache(source).version(), route.version(),
        context.removal_savings(), taxi == source, [&]() -> std::optional<InsertionChoice> {
                // Phase 1b: best-first search with thread_local scratch.
                thread_local ParcelScratch scratch;
                auto& pickup_cost    = scratch.pickup_cost;
                auto& delivery_cost  = scratch.delivery_cost;
                auto& adjacent_cost  = scratch.adjacent_cost;
                auto& suffix_delivery = scratch.suffix_delivery;
                auto& pairs          = scratch.pairs;

                const auto length = context.length(taxi);
                const auto& in = state.instance();
                const auto loc = state.location(r);
                const RouteView view{route.nodes(), loc.pickup, loc.dropoff,
                                     taxi == source};
                const auto pickup_node    = state.pickup_node(r);
                const auto delivery_node  = in.parcel_dropoff(r - state.passenger_count());
                const auto savings = context.removal_savings();

                // Step 1: pre-compute cost arrays O(L) once.
                pickup_cost.resize(length + 1);
                delivery_cost.resize(length + 1);
                adjacent_cost.resize(length + 1);
                suffix_delivery.assign(length + 2,
                                       std::numeric_limits<double>::infinity());
                for (std::size_t pos = 0; pos <= length; ++pos) {
                    pickup_cost[pos]   = single_insertion_cost(in, taxi, view, pos, pickup_node);
                    delivery_cost[pos] = single_insertion_cost(in, taxi, view, pos, delivery_node);
                    adjacent_cost[pos] = adjacent_insertion_cost(in, taxi, view, pos,
                                             pickup_node, delivery_node);
                }
                for (std::size_t pos = length + 1; pos-- > 0;)
                    suffix_delivery[pos] = std::min(delivery_cost[pos],
                                                    suffix_delivery[pos + 1]);

                // Step 2: collect (a,b) pairs whose objective >= kMinGain.
                // Outer-loop pruning via suffix_delivery avoids inner loop entirely
                // when no (a,b>=a) can beat the threshold.
                pairs.clear();
                for (std::size_t a = 0; a <= length; ++a) {
                    const auto best_delta = std::min(adjacent_cost[a],
                        pickup_cost[a] + suffix_delivery[a + 1]);
                    if (savings - best_delta + kTolerance < kMinGain) continue;
                    for (std::size_t b = a; b <= length; ++b) {
                        const auto delta = b == a ? adjacent_cost[a]
                                                  : pickup_cost[a] + delivery_cost[b];
                        const auto objective = savings - delta;
                        if (objective + kTolerance < kMinGain) continue;
                        pairs.push_back({objective, a, b});
                    }
                }
                if (pairs.empty()) return std::nullopt;

                // Step 3: sort best-first, call FastRouteCheck only when needed.
                // Tie-break (a,b) ascending for deterministic results.
                std::sort(pairs.begin(), pairs.end(),
                          [](const PairCandidate& x, const PairCandidate& y) {
                              if (x.objective != y.objective)
                                  return x.objective > y.objective;
                              if (x.pickup != y.pickup)
                                  return x.pickup < y.pickup;
                              return x.delivery < y.delivery;
                          });

                // Phase 1b: reuse pickup_prefix across pairs with the same pickup pos.
                std::size_t prefix_a = std::numeric_limits<std::size_t>::max();
                std::optional<FastRouteCheck> prefix;
                for (const auto& pair : pairs) {
                    if (pair.pickup != prefix_a) {
                        prefix_a = pair.pickup;
                        if (auto built = context.pickup_prefix(taxi, route, pair.pickup))
                            prefix.emplace(*built);
                        else
                            prefix.reset();
                    }
                    if (!prefix) continue;
                    if (const auto gain = context.gain_from_prefix(
                            taxi, route, pair.pickup, pair.delivery, *prefix)) {
                        // Phase 1c: assert pair.objective ≈ *gain (two independent code paths).
                        assert(std::abs(*gain - pair.objective)
                               < 1e-6 * std::max(1.0, std::abs(*gain)));
                        return InsertionChoice{r, taxi, pair.pickup, pair.delivery, *gain};
                    }
                }
                return std::nullopt;
            });
}

bool ParcelRelocate::improve(SearchState& state) const {
    RelocationPool pool;
    const auto scan_request = [&](std::size_t r, bool all,
                                  std::size_t first, std::size_t second) {
        if (state.location(r).taxi < 0) return;
        ParcelRelocationContext context(state, r);
        const auto source = static_cast<std::size_t>(state.location(r).taxi);
        if (!context.valid()) return;
        const auto& index = state.empty_taxi_index();
        const bool indexed = index && index->ensure(r);
        if (indexed) {
            if (all) {
                static const RouteCache empty_route;
                for (const auto& choice : index->choices(r)) {
                    if (state.is_active(choice.taxi)) continue;
                    const auto gain = context.gain(choice.taxi, empty_route, 0, 0);
                    if (gain) pool.add(InsertionChoice{r, choice.taxi, 0, 0, *gain}, source);
                    break;
                }
            }
            for (const auto& [taxi, current] : state.active_routes())
                if (all || taxi == first || taxi == second)
                    pool.add(best_on_taxi(state, context, r, taxi, current), source);
        } else {
            for (std::size_t taxi = 0; taxi < state.taxi_count(); ++taxi)
                if (all || taxi == first || taxi == second)
                    pool.add(best_on_taxi(state, context, r, taxi, state.cache(taxi)), source);
        }
    };
    const auto scan_all = [&] {
        pool.clear();
        for (std::size_t r = state.passenger_count(); r < state.request_count(); ++r)
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
            throw std::logic_error("parcel relocation descriptor disagrees with mutation");
        changed = true;
        if (state.is_active(source) != source_active_before ||
            state.is_active(target) != target_active_before) {
            scan_all();
            continue;
        }
        pool.erase_affected(source, target);
        for (std::size_t r = state.passenger_count(); r < state.request_count(); ++r) {
            if (state.location(r).taxi < 0) continue;
            const auto origin = static_cast<std::size_t>(state.location(r).taxi);
            scan_request(r, origin == source || origin == target, source, target);
        }
    }
    return changed;
}

} // namespace soict::alns
