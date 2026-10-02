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
    
    // thread_local scratch to avoid per-call allocations
    thread_local std::vector<double> pickup_cost, delivery_cost, adjacent_cost, suffix_delivery;
    pickup_cost.resize(length + 1);
    delivery_cost.resize(length + 1);
    adjacent_cost.resize(length + 1);

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

void SwapRequest::evaluate_pair(const SearchState& state,
                                std::size_t r1, std::size_t r2,
                                double s1, double s2,
                                double best_gain,
                                SwapPool& pool) const {
    const auto& in = state.instance();
    const auto loc1 = state.location(r1);
    const auto loc2 = state.location(r2);
    const auto t1 = static_cast<std::size_t>(loc1.taxi);
    const auto t2 = static_cast<std::size_t>(loc2.taxi);
    if (t1 == t2) return;

    // We cannot use s1 + s2 < best_gain because lb_delta can be negative!
    // Fast cache check
    if (cache_[t2][r1].count == 0 || cache_[t1][r2].count == 0) return;

    double lb_delta1 = cache_[t2][r1].entries[0].delta;
    double lb_delta2 = cache_[t1][r2].entries[0].delta;
    
    if (s1 + s2 - lb_delta1 - lb_delta2 + kTolerance < best_gain) return;

    // Fast cache check
    if (cache_[t2][r1].count == 0 || cache_[t1][r2].count == 0) return;

    // Exact insertion cost for r1 into t2 (with r2 removed)
    const RouteView view2{state.cache(t2).nodes(), loc2.pickup, loc2.dropoff, true};
    const auto len2 = view2.size();
    const auto p_node1 = state.pickup_node(r1);
    const auto d_node1 = r1 < state.passenger_count() ? in.passenger_dropoff(r1) : in.parcel_dropoff(r1 - state.passenger_count());

    struct Cand { std::size_t p, d; double delta; };
    thread_local std::vector<Cand> cands1, cands2;
    cands1.clear(); cands2.clear();

    if (r1 < state.passenger_count()) {
        for (std::size_t a = 0; a <= len2; ++a) {
            cands1.push_back({a, a, adjacent_insertion_cost(in, t2, view2, a, p_node1, d_node1)});
        }
    } else {
        for (std::size_t a = 0; a <= len2; ++a) {
            const double dp = single_insertion_cost(in, t2, view2, a, p_node1);
            for (std::size_t b = a; b <= len2; ++b) {
                const double d = b == a ? adjacent_insertion_cost(in, t2, view2, a, p_node1, d_node1) :
                                 dp + single_insertion_cost(in, t2, view2, b, d_node1);
                cands1.push_back({a, b, d});
            }
        }
    }
    
    std::sort(cands1.begin(), cands1.end(), [](const auto& a, const auto& b) { return a.delta < b.delta; });
    if (cands1.empty() || s1 + s2 - cands1[0].delta - lb_delta2 + kTolerance < best_gain) return;

    // Exact insertion cost for r2 into t1 (with r1 removed)
    const auto& route1 = state.cache(t1);
    const RouteView view1{route1.nodes(), loc1.pickup, loc1.dropoff, true};
    const auto len1 = view1.size();
    const auto p_node2 = state.pickup_node(r2);
    const auto d_node2 = r2 < state.passenger_count() ? in.passenger_dropoff(r2) : in.parcel_dropoff(r2 - state.passenger_count());

    if (r2 < state.passenger_count()) {
        for (std::size_t a = 0; a <= len1; ++a) {
            cands2.push_back({a, a, adjacent_insertion_cost(in, t1, view1, a, p_node2, d_node2)});
        }
    } else {
        for (std::size_t a = 0; a <= len1; ++a) {
            const double dp = single_insertion_cost(in, t1, view1, a, p_node2);
            for (std::size_t b = a; b <= len1; ++b) {
                const double d = b == a ? adjacent_insertion_cost(in, t1, view1, a, p_node2, d_node2) :
                                 dp + single_insertion_cost(in, t1, view1, b, d_node2);
                cands2.push_back({a, b, d});
            }
        }
    }
    
    std::sort(cands2.begin(), cands2.end(), [](const auto& a, const auto& b) { return a.delta < b.delta; });
    if (cands2.empty() || s1 + s2 - cands1[0].delta - cands2[0].delta + kTolerance < best_gain) return;

    // FastRouteCheck both directions (best-first)
    auto map_pos1 = [&](std::size_t p) { return p < loc1.pickup ? p : p + 2; };
    auto map_pos2 = [&](std::size_t p) { return p < loc2.pickup ? p : p + 2; };

    for (const auto& c1 : cands1) {
        for (const auto& c2 : cands2) {
            const double exact_gain = s1 + s2 - c1.delta - c2.delta;
            if (exact_gain <= best_gain) break; // since c2 is sorted, subsequent c2 won't beat best_gain

            std::size_t exact_p1 = c1.p, exact_d1 = c1.d;
            std::size_t exact_p2 = c2.p, exact_d2 = c2.d;

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

            pool.add({r1, r2, t1, t2, exact_p1, exact_d1, exact_p2, exact_d2, exact_gain});
            return; // We found the best feasible insertion for this pair!
        }
    }
}

bool SwapRequest::improve(SearchState& state) const {
    bool changed = false;

    // Ensure cache is up to date for all active taxis
    for (std::size_t taxi = 0; taxi < state.taxi_count(); ++taxi) {
        if (state.is_active(taxi)) update_cache(state, taxi);
    }

    // Collect served requests and their savings, sorted descending
    thread_local std::vector<std::pair<double, std::size_t>> served;
    served.clear();
    for (std::size_t r = 0; r < state.request_count(); ++r) {
        if (state.location(r).taxi < 0) continue;
        const auto g = state.removal_gain(r);
        if (g) served.emplace_back(*g + state.revenue(r), r);
    }
    std::sort(served.begin(), served.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });

    // Build an index: taxi → list of (savings, request) for incremental re-scan
    thread_local std::vector<std::vector<std::size_t>> requests_on_taxi;
    requests_on_taxi.assign(state.taxi_count(), {});
    for (const auto& [s, r] : served) {
        const auto t = static_cast<std::size_t>(state.location(r).taxi);
        requests_on_taxi[t].push_back(r);
    }

    // Initial full scan: evaluate all pairs
    SwapPool pool;
    for (std::size_t i = 0; i < served.size(); ++i) {
        const auto [s1, r1] = served[i];
        for (std::size_t j = i + 1; j < served.size(); ++j) {
            const auto [s2, r2] = served[j];
            if (s1 + s2 + kTolerance < kMinGain) break; // B&B prune
            evaluate_pair(state, r1, r2, s1, s2, kMinGain, pool);
        }
    }

    // Apply best swaps incrementally
    while (const auto desc = pool.best()) {
        const auto t1 = desc->t1;
        const auto t2 = desc->t2;

        // Apply the swap
        state.remove(desc->r1);
        state.remove(desc->r2);
        state.insert(desc->r2, t1, desc->p2, desc->d2);
        state.insert(desc->r1, t2, desc->p1, desc->d1);
        changed = true;

        // Invalidate descriptors involving the 2 changed taxis
        pool.erase_affected(state, t1, t2);

        // Update cache for the 2 changed taxis
        update_cache(state, t1);
        update_cache(state, t2);

        // Rebuild served list and requests_on_taxi for affected taxis
        served.clear();
        for (std::size_t r = 0; r < state.request_count(); ++r) {
            if (state.location(r).taxi < 0) continue;
            const auto g = state.removal_gain(r);
            if (g) served.emplace_back(*g + state.revenue(r), r);
        }
        std::sort(served.begin(), served.end(),
                  [](const auto& a, const auto& b) { return a.first > b.first; });

        requests_on_taxi.assign(state.taxi_count(), {});
        for (const auto& [s, r] : served) {
            const auto t = static_cast<std::size_t>(state.location(r).taxi);
            requests_on_taxi[t].push_back(r);
        }

        // Re-scan only pairs involving requests on t1 or t2
        const double current_best = pool.empty() ? kMinGain :
            pool.best()->gain;

        for (std::size_t affected_taxi : {t1, t2}) {
            for (std::size_t r1 : requests_on_taxi[affected_taxi]) {
                const auto g1 = state.removal_gain(r1);
                if (!g1) continue;
                const double s1 = *g1;

                for (const auto& [s2, r2] : served) {
                    if (r1 == r2) continue;
                    const auto other_taxi = static_cast<std::size_t>(state.location(r2).taxi);
                    if (affected_taxi == other_taxi) continue; // same taxi
                    // Only evaluate if we haven't already (avoid double-counting)
                    // When affected_taxi == t1, skip if r2 is on t2 AND t2 < t1
                    // (it will be covered when affected_taxi == t2)
                    if (affected_taxi == t1 && other_taxi == t2) continue;

                    evaluate_pair(state, r1, r2, s1, s2, kMinGain, pool);
                }
            }
        }
    }

    return changed;
}

} // namespace soict::alns
