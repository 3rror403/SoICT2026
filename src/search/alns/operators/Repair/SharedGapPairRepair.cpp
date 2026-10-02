#include "search/alns/operators/Repair/SharedGapPairRepair.hpp"
#include "search/alns/operators/Repair/GreedyInsertion.hpp"
#include "search/alns/operators/Repair/InsertionSearch.hpp"
#include "search/alns/RouteCache.hpp"
#include <algorithm>
#include <iostream>
#include <cassert>
#include <numeric>

namespace soict::alns {

struct BlockSegment {
    std::size_t first_node;
    std::size_t last_node;
    double internal_cost;
    Segment segment;
    bool feasible;
};

struct PatternInfo {
    std::array<std::size_t, 4> nodes;
    std::size_t v_p_pos;
    std::size_t v_d_pos;
};

using PatternArray = std::array<PatternInfo, 6>;

// Generates valid sequence of 4 nodes for request u and v.
static std::size_t get_valid_patterns(const Instance& instance, std::size_t u, std::size_t v, PatternArray& patterns) {
    const bool u_pass = (u < instance.passengers.size());
    const bool v_pass = (v < instance.passengers.size());
    
    const std::size_t u_p = u_pass ? instance.passenger_pickup(u) : instance.parcel_pickup(u - instance.passengers.size());
    const std::size_t u_d = u_pass ? instance.passenger_dropoff(u) : instance.parcel_dropoff(u - instance.passengers.size());
    const std::size_t v_p = v_pass ? instance.passenger_pickup(v) : instance.parcel_pickup(v - instance.passengers.size());
    const std::size_t v_d = v_pass ? instance.passenger_dropoff(v) : instance.parcel_dropoff(v - instance.passengers.size());

    std::size_t count = 0;
    if (u_pass && v_pass) {
        patterns[count++] = { {u_p, u_d, v_p, v_d}, 2, 3 };
        patterns[count++] = { {v_p, v_d, u_p, u_d}, 0, 1 };
    } else if (u_pass && !v_pass) {
        patterns[count++] = { {u_p, u_d, v_p, v_d}, 2, 3 };
        patterns[count++] = { {v_p, v_d, u_p, u_d}, 0, 1 };
        patterns[count++] = { {v_p, u_p, u_d, v_d}, 0, 3 };
    } else if (!u_pass && v_pass) {
        patterns[count++] = { {v_p, v_d, u_p, u_d}, 0, 1 };
        patterns[count++] = { {u_p, u_d, v_p, v_d}, 2, 3 };
        patterns[count++] = { {u_p, v_p, v_d, u_d}, 1, 2 };
    } else { // both parcels
        patterns[count++] = { {u_p, u_d, v_p, v_d}, 2, 3 };
        patterns[count++] = { {v_p, v_d, u_p, u_d}, 0, 1 };
        patterns[count++] = { {u_p, v_p, u_d, v_d}, 1, 3 };
        patterns[count++] = { {u_p, v_p, v_d, u_d}, 1, 2 };
        patterns[count++] = { {v_p, u_p, v_d, u_d}, 0, 2 };
        patterns[count++] = { {v_p, u_p, u_d, v_d}, 0, 3 };
    }
    return count;
}

static BlockSegment create_block(const Instance& instance, const std::array<std::size_t, 4>& pattern) {
    BlockSegment block;
    block.first_node = pattern[0];
    block.last_node = pattern[3];
    block.internal_cost = 0;
    
    Segment s = single_node(instance, pattern[0]);
    for (int i = 1; i < 4; ++i) {
        block.internal_cost += instance.cost(pattern[i-1], pattern[i]);
        s = join(instance, s, single_node(instance, pattern[i]));
        if (!s.feasible) break;
    }
    block.segment = s;
    block.feasible = s.feasible;
    return block;
}

SharedGapPairRepair::SharedGapPairRepair(const Instance& instance) {
    precompute_partner_bank(instance);
}

void SharedGapPairRepair::precompute_partner_bank(const Instance& instance) {
    const std::size_t n = instance.passengers.size() + instance.parcels.size();
    partner_bank_.resize(n);
    
    for (std::size_t u = 0; u < n; ++u) {
        struct Partner {
            std::size_t v;
            double score;
        };
        std::vector<Partner> candidates;
        candidates.reserve(n - 1);
        
        const bool u_pass = (u < instance.passengers.size());
        const std::size_t loc_u_p = u_pass ? instance.passenger_pickup(u) : instance.parcel_pickup(u - instance.passengers.size());
        const std::size_t loc_u_d = u_pass ? instance.passenger_dropoff(u) : instance.parcel_dropoff(u - instance.passengers.size());
        
        for (std::size_t v = 0; v < n; ++v) {
            if (u == v) continue;
            const bool v_pass = (v < instance.passengers.size());
            const std::size_t loc_v_p = v_pass ? instance.passenger_pickup(v) : instance.parcel_pickup(v - instance.passengers.size());
            const std::size_t loc_v_d = v_pass ? instance.passenger_dropoff(v) : instance.parcel_dropoff(v - instance.passengers.size());
            
            // Simple score: sum of distance between pickups and dropoffs
            // Lower distance = better score
            double dist = instance.time(loc_u_p, loc_v_p) +
                          instance.time(loc_u_d, loc_v_d);
                          
            candidates.push_back({v, dist});
        }
        
        std::sort(candidates.begin(), candidates.end(), 
                  [](const Partner& a, const Partner& b) { return a.score < b.score; });
                  
        for (std::size_t i = 0; i < std::min(BANK_SIZE, candidates.size()); ++i) {
            partner_bank_[u].push_back(candidates[i].v);
        }
    }
}

struct PairDesc {
    std::size_t u, v, taxi, gap, pat_idx;
    double gain, synergy;
    bool exists() const { return u != std::numeric_limits<std::size_t>::max(); }
};

struct SingleDesc {
    std::size_t request, taxi, pickup, delivery;
    double gain;
    bool exists() const { return request != std::numeric_limits<std::size_t>::max(); }
};

void SharedGapPairRepair::apply(SearchState& state, std::span<const std::size_t> removed,
                                std::mt19937_64& rng) const {
    std::vector<std::size_t> unassigned(removed.begin(), removed.end());
    unassigned.erase(std::remove_if(unassigned.begin(), unassigned.end(), [&](std::size_t r) {
        return state.location(r).taxi >= 0;
    }), unassigned.end());

    std::vector<std::size_t> active_taxis;
    std::vector<std::size_t> inactive_taxis;
    std::vector<std::size_t> top_inactive;
    std::vector<double> single_gain(state.request_count());
    std::vector<SingleDesc> single_descs(state.request_count());
    PatternArray patterns;

    constexpr double EPS = 1e-8;

    int pair_search_rounds = 0;

    while (true) {
        SingleDesc best_single{std::numeric_limits<std::size_t>::max(), 0, 0, 0, -std::numeric_limits<double>::infinity()};
        std::fill(single_gain.begin(), single_gain.end(), -std::numeric_limits<double>::infinity());
        
        for (std::size_t req : unassigned) {
            auto choice = best_insertion(state, req);
            if (choice) {
                single_gain[req] = choice->gain;
                single_descs[req] = {req, choice->taxi, choice->pickup, choice->delivery, choice->gain};
                if (choice->gain > best_single.gain && choice->gain > EPS) {
                    best_single = single_descs[req];
                }
            }
        }

        PairDesc best_pair{std::numeric_limits<std::size_t>::max(), 0, 0, 0, 0, 
                           -std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()};
        
        if (pair_search_rounds < 2) {
            ++pair_search_rounds;
            active_taxis.clear();
            inactive_taxis.clear();
        for (std::size_t t = 0; t < state.instance().taxis.size(); ++t) {
            if (state.cache(t).nodes().empty()) {
                inactive_taxis.push_back(t);
            } else {
                active_taxis.push_back(t);
            }
        }
        
        for (std::size_t u : unassigned) {
            for (std::size_t v : unassigned) {
                if (u >= v) continue; 
                
                const std::size_t pat_count = get_valid_patterns(state.instance(), u, v, patterns);
                for (std::size_t pat_idx = 0; pat_idx < pat_count; ++pat_idx) {
                    const auto& pat = patterns[pat_idx];
                    auto block = create_block(state.instance(), pat.nodes);
                    if (!block.feasible) continue;
                    
                    top_inactive = inactive_taxis;
                    if (top_inactive.size() > 3) {
                        std::nth_element(top_inactive.begin(), top_inactive.begin() + 3, top_inactive.end(), 
                            [&](std::size_t t1, std::size_t t2) {
                                return state.instance().time(t1, pat.nodes[0]) < state.instance().time(t2, pat.nodes[0]);
                            });
                        top_inactive.resize(3);
                    }

                    for (std::size_t taxi : active_taxis) {
                        const auto& route = state.cache(taxi);
                        const auto& nodes = route.nodes();
                        for (std::size_t gap = 0; gap <= nodes.size(); ++gap) {
                            if (!route.boundary_allowed(state.instance(), gap)) continue;
                            
                            double delta = 0.0;
                            if (gap == nodes.size()) {
                                std::size_t prev_node = (gap == 0) ? taxi : nodes[gap - 1];
                                delta = state.instance().cost(prev_node, block.first_node) 
                                      + block.internal_cost;
                            } else {
                                std::size_t prev_node = (gap == 0) ? taxi : nodes[gap - 1];
                                std::size_t next_node = nodes[gap];
                                delta = state.instance().cost(prev_node, block.first_node) 
                                      + block.internal_cost 
                                      + state.instance().cost(block.last_node, next_node) 
                                      - state.instance().cost(prev_node, next_node);
                            }
                            
                            double gain = state.revenue(u) + state.revenue(v) - delta;
                            if (gain <= EPS) continue;
                            
                            double synergy = gain - std::max(0.0, single_gain[u]) - std::max(0.0, single_gain[v]);
                            if (synergy <= best_pair.synergy + EPS) continue;

                            RouteCandidate candidate(state.instance(), taxi, route);
                            candidate.add_range(0, gap);
                            candidate.add_segment(block.segment);
                            candidate.add_range(gap, nodes.size());
                            
                            auto cost = candidate.cost_if_feasible();
                            if (cost) {
                                best_pair = {u, v, taxi, gap, pat_idx, gain, synergy};
                            }
                        }
                    }

                    for (std::size_t taxi : top_inactive) {
                        double delta = state.instance().cost(taxi, block.first_node) 
                                     + block.internal_cost;
                        
                        double gain = state.revenue(u) + state.revenue(v) - delta;
                        if (gain <= EPS) continue;
                        
                        double synergy = gain - std::max(0.0, single_gain[u]) - std::max(0.0, single_gain[v]);
                        if (synergy <= best_pair.synergy + EPS) continue;

                        RouteCandidate candidate(state.instance(), taxi, state.cache(taxi));
                        candidate.add_segment(block.segment);
                        auto cost = candidate.cost_if_feasible();
                        if (cost) {
                            best_pair = {u, v, taxi, 0, pat_idx, gain, synergy};
                        }
                    }
                }
            }
        }
        } // End if pair_search_rounds < 2

        if (!best_single.exists() && !best_pair.exists()) {
            break;
        }

        bool applied = false;

        if (best_pair.exists() && best_pair.synergy > EPS) {
            bool conflict = false;
            if (best_single.exists() && best_single.taxi == best_pair.taxi) {
                conflict = true;
            }
            if (!best_single.exists() || best_single.request == best_pair.u || 
                best_single.request == best_pair.v || conflict) {
                
                const std::size_t pat_count = get_valid_patterns(state.instance(), best_pair.u, best_pair.v, patterns);
                assert(best_pair.pat_idx < pat_count);
                auto block_nodes = patterns[best_pair.pat_idx].nodes;
                
                bool success = state.insert_block(best_pair.taxi, best_pair.gap, block_nodes, best_pair.gain);
                if (success) {
                    unassigned.erase(std::remove(unassigned.begin(), unassigned.end(), best_pair.u), unassigned.end());
                    unassigned.erase(std::remove(unassigned.begin(), unassigned.end(), best_pair.v), unassigned.end());
                    applied = true;
                }
            }
        }

        if (!applied && best_single.exists()) {
            bool success = state.insert(best_single.request, best_single.taxi, best_single.pickup, best_single.delivery);
            if (success) {
                unassigned.erase(std::remove(unassigned.begin(), unassigned.end(), best_single.request), unassigned.end());
                applied = true;
            }
        }

        if (!applied && best_pair.exists() && best_pair.gain > EPS) {
            const std::size_t pat_count = get_valid_patterns(state.instance(), best_pair.u, best_pair.v, patterns);
            
            if (best_pair.pat_idx < pat_count) {
                const auto& block_nodes = patterns[best_pair.pat_idx].nodes;
                
                if (state.insert_block(best_pair.taxi, best_pair.gap, block_nodes, best_pair.gain)) {
                    unassigned.erase(std::remove(unassigned.begin(), unassigned.end(), best_pair.u), unassigned.end());
                    unassigned.erase(std::remove(unassigned.begin(), unassigned.end(), best_pair.v), unassigned.end());
                    applied = true;
                }
            }
        }

        if (!applied) break;
    }
}

// detect_after_destroy removed

} // namespace soict::alns
