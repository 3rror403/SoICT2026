#include "search/alns/operators/LocalSearch/SwapRequest.hpp"
#include "search/alns/MoveCost.hpp"
#include <algorithm>

namespace soict::alns {

namespace {
constexpr double kMinGain = 1e-9;
constexpr double kTolerance = 1e-8;
} // namespace

void SwapRequest::update_cache(const SearchState& state, std::size_t taxi) const {
    const auto& route = state.cache(taxi);
    const auto version = route.version();
    if (taxi < versions_.size() && versions_[taxi] == version) return;
    
    if (versions_.size() <= taxi) {
        versions_.resize(taxi + 1, 0);
        cache_.resize(taxi + 1);
    }
    
    auto& row = cache_[taxi];
    if (row.size() < state.request_count()) row.resize(state.request_count());
    
    const RouteView view{route.nodes(), 0, 0, false};
    const auto length = view.size();
    const auto& in = state.instance();
    
    std::vector<double> pickup_cost(length + 1), delivery_cost(length + 1),
                        adjacent_cost(length + 1), suffix_delivery(length + 2);

    for (std::size_t r = 0; r < state.request_count(); ++r) {
        row[r].count = 0;
        // Don't cache insertion of a request into its own taxi
        if (state.location(r).taxi == static_cast<int>(taxi)) continue;
        
        if (r < state.passenger_count()) {
            const auto p = state.pickup_node(r);
            const auto d = in.passenger_dropoff(r);
            for (std::size_t a = 0; a <= length; ++a) {
                if (a && !route.boundary_allowed(in, a)) continue;
                const double delta = adjacent_insertion_cost(in, taxi, view, a, p, d);
                row[r].add(a, a, delta);
            }
        } else {
            const auto p = state.pickup_node(r);
            const auto d = in.parcel_dropoff(r - state.passenger_count());
            for (std::size_t pos = 0; pos <= length; ++pos) {
                pickup_cost[pos] = single_insertion_cost(in, taxi, view, pos, p);
                delivery_cost[pos] = single_insertion_cost(in, taxi, view, pos, d);
                adjacent_cost[pos] = adjacent_insertion_cost(in, taxi, view, pos, p, d);
            }
            suffix_delivery.assign(length + 2, std::numeric_limits<double>::infinity());
            for (std::size_t pos = length + 1; pos-- > 0;)
                suffix_delivery[pos] = std::min(delivery_cost[pos], suffix_delivery[pos + 1]);
                
            for (std::size_t a = 0; a <= length; ++a) {
                if (a && !route.boundary_allowed(in, a)) continue;
                for (std::size_t b = a; b <= length; ++b) {
                    if (b && !route.boundary_allowed(in, b)) continue;
                    const double delta = b == a ? adjacent_cost[a] : pickup_cost[a] + delivery_cost[b];
                    row[r].add(a, b, delta);
                }
            }
        }
    }
    versions_[taxi] = version;
}

bool SwapRequest::improve(SearchState& state) const {
    bool changed = false;
    for (;;) {
        for (std::size_t taxi = 0; taxi < state.taxi_count(); ++taxi) {
            if (state.is_active(taxi)) update_cache(state, taxi);
        }

        std::vector<std::size_t> served;
        served.reserve(state.request_count());
        for (std::size_t r = 0; r < state.request_count(); ++r) {
            if (state.location(r).taxi >= 0) served.push_back(r);
        }

        // Sort requests by removal savings to allow Branch & Bound
        std::sort(served.begin(), served.end(), [&](std::size_t a, std::size_t b) {
            return state.removal_gain(a).value_or(-1e18) > state.removal_gain(b).value_or(-1e18);
        });

        std::size_t best_r1 = 0, best_r2 = 0;
        std::size_t best_p1 = 0, best_d1 = 0, best_p2 = 0, best_d2 = 0;
        double best_gain = kMinGain;
        
        const auto& in = state.instance();

        for (std::size_t i = 0; i < served.size(); ++i) {
            const auto r1 = served[i];
            const auto loc1 = state.location(r1);
            const auto t1 = static_cast<std::size_t>(loc1.taxi);
            const auto removal_gain1 = state.removal_gain(r1);
            if (!removal_gain1) continue;
            const double s1 = *removal_gain1;
            const auto& route1 = state.cache(t1);

            for (std::size_t j = i + 1; j < served.size(); ++j) {
                const auto r2 = served[j];
                const auto loc2 = state.location(r2);
                const auto t2 = static_cast<std::size_t>(loc2.taxi);
                if (t1 == t2) continue; // Must be different routes
                
                const auto removal_gain2 = state.removal_gain(r2);
                if (!removal_gain2) continue;
                const double s2 = *removal_gain2;
                const double upper_bound = s1 + s2;
                if (upper_bound + kTolerance < best_gain) break; // Prune inner loop!
                
                // Fast cache check (approximate lower bound on insertion cost)
                if (cache_[t2][r1].count == 0 || cache_[t1][r2].count == 0) continue;
                
                // If the overlap is bad, exact cost might be slightly higher or lower, 
                // but usually the cache cost is a good estimate.
                const double cache_delta1 = cache_[t2][r1].entries[0].delta;
                const double cache_delta2 = cache_[t1][r2].entries[0].delta;
                const double approx_gain = upper_bound - cache_delta1 - cache_delta2;
                
                // We test exactly if approx_gain + margin > best_gain, or we just test everything that wasn't pruned.
                // Since exact test is cheap, let's just do exact test for all unpruned!
                
                const RouteView view2{state.cache(t2).nodes(), loc2.pickup, loc2.dropoff, true};
                const auto len2 = view2.size();
                const auto p_node1 = state.pickup_node(r1);
                const auto d_node1 = r1 < state.passenger_count() ? in.passenger_dropoff(r1) : in.parcel_dropoff(r1 - state.passenger_count());
                
                double exact_delta1 = std::numeric_limits<double>::infinity();
                std::size_t exact_p1 = 0, exact_d1 = 0;
                
                if (r1 < state.passenger_count()) {
                    for (std::size_t a = 0; a <= len2; ++a) {
                        const double d = adjacent_insertion_cost(in, t2, view2, a, p_node1, d_node1);
                        if (d < exact_delta1) { exact_delta1 = d; exact_p1 = a; exact_d1 = a; }
                    }
                } else {
                    for (std::size_t a = 0; a <= len2; ++a) {
                        const double dp = single_insertion_cost(in, t2, view2, a, p_node1);
                        for (std::size_t b = a; b <= len2; ++b) {
                            const double d = b == a ? adjacent_insertion_cost(in, t2, view2, a, p_node1, d_node1) :
                                             dp + single_insertion_cost(in, t2, view2, b, d_node1);
                            if (d < exact_delta1) { exact_delta1 = d; exact_p1 = a; exact_d1 = b; }
                        }
                    }
                }
                
                if (upper_bound - exact_delta1 + kTolerance < best_gain) continue;
                
                const RouteView view1{route1.nodes(), loc1.pickup, loc1.dropoff, true};
                const auto len1 = view1.size();
                const auto p_node2 = state.pickup_node(r2);
                const auto d_node2 = r2 < state.passenger_count() ? in.passenger_dropoff(r2) : in.parcel_dropoff(r2 - state.passenger_count());
                
                double exact_delta2 = std::numeric_limits<double>::infinity();
                std::size_t exact_p2 = 0, exact_d2 = 0;
                
                if (r2 < state.passenger_count()) {
                    for (std::size_t a = 0; a <= len1; ++a) {
                        const double d = adjacent_insertion_cost(in, t1, view1, a, p_node2, d_node2);
                        if (d < exact_delta2) { exact_delta2 = d; exact_p2 = a; exact_d2 = a; }
                    }
                } else {
                    for (std::size_t a = 0; a <= len1; ++a) {
                        const double dp = single_insertion_cost(in, t1, view1, a, p_node2);
                        for (std::size_t b = a; b <= len1; ++b) {
                            const double d = b == a ? adjacent_insertion_cost(in, t1, view1, a, p_node2, d_node2) :
                                             dp + single_insertion_cost(in, t1, view1, b, d_node2);
                            if (d < exact_delta2) { exact_delta2 = d; exact_p2 = a; exact_d2 = b; }
                        }
                    }
                }
                
                const double exact_gain = upper_bound - exact_delta1 - exact_delta2;
                if (exact_gain > best_gain) {
                    // FastRouteCheck both
                    auto map_pos1 = [&](std::size_t p) { return p < loc1.pickup ? p : p + 2; };
                    auto map_pos2 = [&](std::size_t p) { return p < loc2.pickup ? p : p + 2; };
                    
                    RouteCandidate cand1(in, t1, route1);
                    auto add_range1 = [&](std::size_t first, std::size_t end) {
                        const auto left_end = std::min(end, loc1.pickup);
                        if (first < left_end) cand1.add_range(first, left_end);
                        const auto right_start = std::max(first, loc1.pickup);
                        if (right_start < end) cand1.add_range(right_start + 2, end + 2);
                    };
                    add_range1(0, exact_p2); cand1.add_node(p_node2);
                    if (exact_p2 == exact_d2) cand1.add_node(d_node2);
                    add_range1(exact_p2, exact_d2);
                    if (exact_p2 != exact_d2) cand1.add_node(d_node2);
                    add_range1(exact_d2, len1);
                    
                    if (exact_p2 && !route1.boundary_allowed(in, map_pos1(exact_p2 - 1) + 1)) continue;
                    if (exact_d2 != exact_p2 && !route1.boundary_allowed(in, map_pos1(exact_d2 - 1) + 1)) continue;
                    if (!cand1.cost_if_feasible()) continue;
                    
                    RouteCandidate cand2(in, t2, state.cache(t2));
                    auto add_range2 = [&](std::size_t first, std::size_t end) {
                        const auto left_end = std::min(end, loc2.pickup);
                        if (first < left_end) cand2.add_range(first, left_end);
                        const auto right_start = std::max(first, loc2.pickup);
                        if (right_start < end) cand2.add_range(right_start + 2, end + 2);
                    };
                    add_range2(0, exact_p1); cand2.add_node(p_node1);
                    if (exact_p1 == exact_d1) cand2.add_node(d_node1);
                    add_range2(exact_p1, exact_d1);
                    if (exact_p1 != exact_d1) cand2.add_node(d_node1);
                    add_range2(exact_d1, len2);
                    
                    if (exact_p1 && !state.cache(t2).boundary_allowed(in, map_pos2(exact_p1 - 1) + 1)) continue;
                    if (exact_d1 != exact_p1 && !state.cache(t2).boundary_allowed(in, map_pos2(exact_d1 - 1) + 1)) continue;
                    if (!cand2.cost_if_feasible()) continue;
                    
                    best_gain = exact_gain;
                    best_r1 = r1; best_r2 = r2;
                    best_p1 = exact_p1; best_d1 = exact_d1;
                    best_p2 = exact_p2; best_d2 = exact_d2;
                }
            }
        }
        
        if (best_gain > kMinGain) {
            const auto t1 = static_cast<std::size_t>(state.location(best_r1).taxi);
            const auto t2 = static_cast<std::size_t>(state.location(best_r2).taxi);
            state.remove(best_r1);
            state.remove(best_r2);
            state.insert(best_r2, t1, best_p2, best_d2);
            state.insert(best_r1, t2, best_p1, best_d1);
            changed = true;
        } else {
            break;
        }
    }
    return changed;
}

} // namespace soict::alns
