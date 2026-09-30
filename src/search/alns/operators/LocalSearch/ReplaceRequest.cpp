#include "search/alns/operators/LocalSearch/ReplaceRequest.hpp"
#include "search/alns/MoveCost.hpp"
#include <algorithm>
#include <limits>
#include <optional>
#include <vector>
#include <cassert>

namespace soict::alns {

namespace {
constexpr double kMinGain = 1e-9;
constexpr double kTolerance = 1e-8;
} // namespace

bool ReplaceRequest::improve(SearchState& state) const {
    bool changed = false;

    // thread_local scratch to avoid repeated heap allocations
    thread_local std::vector<std::size_t> unserved;
    thread_local std::vector<std::pair<double, std::size_t>> served_by_savings;
    thread_local std::vector<double> pickup_cost, delivery_cost, adjacent_cost, suffix_delivery;

    for (;;) {
        unserved.clear();
        for (std::size_t r = 0; r < state.request_count(); ++r) {
            if (state.location(r).taxi < 0) unserved.push_back(r);
        }
        if (unserved.empty()) break;

        // Sort served requests by removal savings descending for B&B pruning
        served_by_savings.clear();
        for (std::size_t r = 0; r < state.request_count(); ++r) {
            if (state.location(r).taxi < 0) continue;
            const auto g = state.removal_gain(r);
            if (g) served_by_savings.emplace_back(*g, r);
        }
        std::sort(served_by_savings.begin(), served_by_savings.end(),
                  [](const auto& a, const auto& b) { return a.first > b.first; });

        std::size_t best_r_out = 0;
        std::size_t best_r_in = 0;
        std::size_t best_taxi = 0;
        std::size_t best_pickup = 0;
        std::size_t best_delivery = 0;
        double best_gain = kMinGain;
        
        for (const auto& [savings, r_out] : served_by_savings) {
            // B&B: savings is the absolute upper bound for any replacement
            // involving this r_out. If it's below best_gain, all remaining
            // r_out (sorted descending) are also below → break.
            if (savings + kTolerance < best_gain) break;

            const auto loc_out = state.location(r_out);
            const auto taxi = static_cast<std::size_t>(loc_out.taxi);
            
            const auto& route = state.cache(taxi);
            const RouteView view{route.nodes(), loc_out.pickup, loc_out.dropoff, true};
            const auto length = view.size();
            const auto& in = state.instance();
            
            // Reuse thread_local scratch vectors (suffix_delivery is filled per r_in)
            pickup_cost.resize(length + 1);
            delivery_cost.resize(length + 1);
            adjacent_cost.resize(length + 1);

            auto map_pos = [&](std::size_t p) {
                return p < loc_out.pickup ? p : p + 2;
            };
            auto add_range = [&](RouteCandidate& candidate, std::size_t first, std::size_t end) {
                const auto left_end = std::min(end, loc_out.pickup);
                if (first < left_end) candidate.add_range(first, left_end);
                const auto right_start = std::max(first, loc_out.pickup);
                if (right_start < end) candidate.add_range(right_start + 2, end + 2);
            };

            for (std::size_t r_in : unserved) {
                if (r_in < state.passenger_count()) {
                    const auto pickup_node = state.pickup_node(r_in);
                    const auto delivery_node = in.passenger_dropoff(r_in);
                    for (std::size_t a = 0; a <= length; ++a) {
                        const double delta = adjacent_insertion_cost(in, taxi, view, a, pickup_node, delivery_node);
                        const double objective = savings - delta;
                        if (objective + kTolerance < best_gain) continue;
                        
                        if (a && !route.boundary_allowed(in, map_pos(a - 1) + 1)) continue;
                        
                        RouteCandidate candidate(in, taxi, route);
                        add_range(candidate, 0, a);
                        candidate.add_node(pickup_node);
                        candidate.add_node(delivery_node);
                        add_range(candidate, a, length);
                        
                        if (candidate.cost_if_feasible()) {
                            if (objective > best_gain) {
                                best_gain = objective;
                                best_r_out = r_out;
                                best_r_in = r_in;
                                best_taxi = taxi;
                                best_pickup = a;
                                best_delivery = a;
                            }
                        }
                    }
                } else {
                    const auto pickup_node = state.pickup_node(r_in);
                    const auto delivery_node = in.parcel_dropoff(r_in - state.passenger_count());
                    for (std::size_t pos = 0; pos <= length; ++pos) {
                        pickup_cost[pos] = single_insertion_cost(in, taxi, view, pos, pickup_node);
                        delivery_cost[pos] = single_insertion_cost(in, taxi, view, pos, delivery_node);
                        adjacent_cost[pos] = adjacent_insertion_cost(in, taxi, view, pos, pickup_node, delivery_node);
                    }
                    suffix_delivery.assign(length + 2, std::numeric_limits<double>::infinity());
                    for (std::size_t pos = length + 1; pos-- > 0;)
                        suffix_delivery[pos] = std::min(delivery_cost[pos], suffix_delivery[pos + 1]);
                        
                    for (std::size_t a = 0; a <= length; ++a) {
                        const auto best_delta = std::min(adjacent_cost[a], pickup_cost[a] + suffix_delivery[a + 1]);
                        if (savings - best_delta + kTolerance < best_gain) continue;
                        
                        if (a && !route.boundary_allowed(in, map_pos(a - 1) + 1)) continue;

                        RouteCandidate prefix(in, taxi, route);
                        add_range(prefix, 0, a);
                        prefix.add_node(pickup_node);
                        if (!prefix.cost_if_feasible()) continue;
                        
                        for (std::size_t b = a; b <= length; ++b) {
                            const double delta = b == a ? adjacent_cost[a] : pickup_cost[a] + delivery_cost[b];
                            const double objective = savings - delta;
                            if (objective + kTolerance < best_gain) continue;
                            
                            if (b != a && !route.boundary_allowed(in, map_pos(b - 1) + 1)) continue;
                            
                            RouteCandidate candidate = prefix;
                            add_range(candidate, a, b);
                            candidate.add_node(delivery_node);
                            add_range(candidate, b, length);
                            
                            if (candidate.cost_if_feasible()) {
                                if (objective > best_gain) {
                                    best_gain = objective;
                                    best_r_out = r_out;
                                    best_r_in = r_in;
                                    best_taxi = taxi;
                                    best_pickup = a;
                                    best_delivery = b;
                                }
                            }
                        }
                    }
                }
            }
        }
        
        if (best_gain > kMinGain) {
            if (!state.remove(best_r_out))
                throw std::logic_error("replace-request remove failed");
            if (!state.insert(best_r_in, best_taxi, best_pickup, best_delivery))
                throw std::logic_error("replace-request insert failed");
            changed = true;
        } else {
            break;
        }
    }
    return changed;
}

} // namespace soict::alns
