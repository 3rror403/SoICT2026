#include "search/alns/Operators.hpp"

#include <algorithm>
#include <random>
#include <utility>
#include <vector>

namespace soict {
namespace alns {

// ============================================================
// Shared internal helpers
// ============================================================

namespace {

// Enumerate all feasible (taxi, pickup_pos, dropoff_pos) for req_id and
// return the one giving the highest benefit.
// Returns {best_benefit, best_taxi, best_pp, best_dp}.
// best_taxi == -1 means no feasible insertion was found.
struct BestInsertion { double benefit; int taxi, pp, dp; };

BestInsertion find_best_insertion(const State& state, int req_id) {
    const Instance& inst = state.get_instance();
    const int N = static_cast<int>(inst.passengers.size());
    const int K = static_cast<int>(inst.taxis.size());
    BestInsertion best{-1e18, -1, 0, 0};

    for (int k = 0; k < K; ++k) {
        const int sz = static_cast<int>(state.get_solution().routes[k].size());

        if (req_id < N) {
            // Passenger: only need to choose pickup position; dropoff = pp+1.
            for (int pp = 0; pp <= sz; ++pp) {
                double b = state.try_insert(req_id, k, pp, pp + 1);
                if (b > best.benefit) { best = {b, k, pp, pp + 1}; }
            }
        } else {
            // Parcel: try all (pickup, dropoff) pairs where pp < dp.
            for (int pp = 0; pp <= sz; ++pp) {
                for (int dp = pp + 1; dp <= sz + 1; ++dp) {
                    double b = state.try_insert(req_id, k, pp, dp);
                    if (b > best.benefit) { best = {b, k, pp, dp}; }
                }
            }
        }
    }
    return best;
}

} // anonymous namespace

// ============================================================
// RandomRemoval
// ============================================================

void RandomRemoval::destroy(State& state, int num_to_remove) {
    auto assigned = state.get_assigned_requests();
    if (assigned.empty()) return;

    std::mt19937 rng(std::random_device{}());
    std::shuffle(assigned.begin(), assigned.end(), rng);

    const int to_remove = std::min(num_to_remove,
                                   static_cast<int>(assigned.size()));
    for (int i = 0; i < to_remove; ++i) {
        state.remove_request(assigned[i]);
    }
}

// ============================================================
// WorstRemoval
// ============================================================

void WorstRemoval::destroy(State& state, int num_to_remove) {
    const auto assigned = state.get_assigned_requests();
    if (assigned.empty()) return;

    const Instance& inst  = state.get_instance();
    const double cur_ben  = state.benefit();

    // For each assigned request compute its marginal contribution:
    // contribution = current_benefit - benefit_without_req.
    // (Lower contribution → less valuable → remove first.)
    std::vector<std::pair<double, int>> contribs; // (contribution, req_id)
    contribs.reserve(assigned.size());

    for (int req_id : assigned) {
        // Build a solution copy without req_id.
        std::size_t pnode, dnode;
        const int N = static_cast<int>(inst.passengers.size());
        if (req_id < N) {
            pnode = inst.passenger_pickup(req_id);
            dnode = inst.passenger_dropoff(req_id);
        } else {
            pnode = inst.parcel_pickup(req_id - N);
            dnode = inst.parcel_dropoff(req_id - N);
        }

        Solution sol_copy = state.get_solution();
        for (auto& route : sol_copy.routes) {
            route.erase(
                std::remove_if(route.begin(), route.end(),
                    [pnode, dnode](int nd) {
                        return static_cast<std::size_t>(nd) == pnode ||
                               static_cast<std::size_t>(nd) == dnode;
                    }),
                route.end());
        }

        const auto eval = sol_copy.evaluate(inst);
        const double ben_without = eval.feasible ? eval.benefit : 0.0;
        contribs.push_back({cur_ben - ben_without, req_id});
    }

    // Sort ascending by contribution (cheapest to remove first).
    std::sort(contribs.begin(), contribs.end());

    const int to_remove = std::min(num_to_remove,
                                   static_cast<int>(contribs.size()));
    for (int i = 0; i < to_remove; ++i) {
        state.remove_request(contribs[i].second);
    }
}

// ============================================================
// GreedyInsertion
// ============================================================

void GreedyInsertion::repair(State& state) {
    // Snapshot to avoid iterator invalidation: do_insert calls erase() on
    // state.unassigned, which would corrupt a range-for over the same vector.
    std::vector<int> to_process = state.unassigned;
    state.unassigned.clear();           // do_insert won't find anything to erase
    std::vector<int> still_unassigned;

    for (int req_id : to_process) {
        auto best = find_best_insertion(state, req_id);
        if (best.taxi >= 0 && best.benefit > -1e17) {
            bool ok = state.do_insert(req_id, best.taxi, best.pp, best.dp);
            if (!ok) still_unassigned.push_back(req_id);
        } else {
            still_unassigned.push_back(req_id);
        }
    }
    state.unassigned = still_unassigned;
}


// ============================================================
// RandomInsertion
// ============================================================

void RandomInsertion::repair(State& state) {
    const Instance& inst = state.get_instance();
    const int N = static_cast<int>(inst.passengers.size());
    const int K = static_cast<int>(inst.taxis.size());

    std::mt19937 rng(std::random_device{}());

    // Shuffle so insertion order is random.
    std::vector<int> order = state.unassigned;
    std::shuffle(order.begin(), order.end(), rng);

    std::vector<int> still_unassigned;

    for (int req_id : order) {
        // Collect all feasible positions.
        struct Pos { int taxi, pp, dp; };
        std::vector<Pos> feasible_positions;

        for (int k = 0; k < K; ++k) {
            const int sz = static_cast<int>(
                state.get_solution().routes[k].size());

            if (req_id < N) {
                for (int pp = 0; pp <= sz; ++pp) {
                    if (state.try_insert(req_id, k, pp, pp + 1) > -1e17) {
                        feasible_positions.push_back({k, pp, pp + 1});
                    }
                }
            } else {
                for (int pp = 0; pp <= sz; ++pp) {
                    for (int dp = pp + 1; dp <= sz + 1; ++dp) {
                        if (state.try_insert(req_id, k, pp, dp) > -1e17) {
                            feasible_positions.push_back({k, pp, dp});
                        }
                    }
                }
            }
        }

        if (!feasible_positions.empty()) {
            std::uniform_int_distribution<int> pick(
                0, static_cast<int>(feasible_positions.size()) - 1);
            const auto& chosen = feasible_positions[pick(rng)];
            bool ok = state.do_insert(req_id, chosen.taxi,
                                      chosen.pp, chosen.dp);
            if (!ok) still_unassigned.push_back(req_id);
        } else {
            still_unassigned.push_back(req_id);
        }
    }
    state.unassigned = still_unassigned;
}

} // namespace alns
} // namespace soict
