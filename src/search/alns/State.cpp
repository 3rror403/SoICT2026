#include "search/alns/State.hpp"

#include <algorithm>
#include <cstddef>

namespace soict {
namespace alns {

// ---- Constructors ----

State::State(const Instance& instance)
    : instance_(instance), current_benefit_(0.0) {
    solution_.routes.resize(instance.taxis.size());
}

State::State(const Instance& instance, const Solution& init_sol)
    : instance_(instance), solution_(init_sol), current_benefit_(0.0) {
    auto eval = solution_.evaluate(instance);
    current_benefit_ = eval.feasible ? eval.benefit : 0.0;
}

// ---- Private helper ----

void State::get_request_nodes(int req_id,
                               std::size_t& pickup_node,
                               std::size_t& dropoff_node) const {
    const int N = static_cast<int>(instance_.passengers.size());
    if (req_id < N) {
        pickup_node  = instance_.passenger_pickup(req_id);
        dropoff_node = instance_.passenger_dropoff(req_id);
    } else {
        const int j  = req_id - N;
        pickup_node  = instance_.parcel_pickup(j);
        dropoff_node = instance_.parcel_dropoff(j);
    }
}

// ---- Accessors ----

double State::benefit() const { return current_benefit_; }

bool State::is_feasible() const {
    return solution_.evaluate(instance_).feasible;
}

// ---- Initialisation ----

void State::init_all_unassigned() {
    for (auto& r : solution_.routes) r.clear();
    current_benefit_ = 0.0;
    unassigned.clear();
    const int N = static_cast<int>(instance_.passengers.size());
    const int M = static_cast<int>(instance_.parcels.size());
    for (int i = 0; i < N + M; ++i) unassigned.push_back(i);
}

// ---- Mutation helpers ----

std::vector<int> State::get_assigned_requests() const {
    const int N = static_cast<int>(instance_.passengers.size());
    const int M = static_cast<int>(instance_.parcels.size());
    const int K = static_cast<int>(instance_.taxis.size());

    std::vector<bool> seen(N + M, false);
    for (const auto& route : solution_.routes) {
        for (int node : route) {
            if (node >= K && node < K + N) {
                seen[node - K] = true;                  // passenger (by pickup node)
            } else if (node >= K + 2 * N && node < K + 2 * N + M) {
                seen[N + (node - K - 2 * N)] = true;   // parcel (by pickup node)
            }
        }
    }

    std::vector<int> assigned;
    assigned.reserve(N + M);
    for (int r = 0; r < N + M; ++r) {
        if (seen[r]) assigned.push_back(r);
    }
    return assigned;
}

bool State::remove_request(int req_id) {
    std::size_t pickup_node, dropoff_node;
    get_request_nodes(req_id, pickup_node, dropoff_node);

    for (auto& route : solution_.routes) {
        // Search for the pickup node; its presence confirms this is the right route.
        bool found_pickup = false;
        for (int i = static_cast<int>(route.size()) - 1; i >= 0; --i) {
            if (static_cast<std::size_t>(route[i]) == pickup_node) {
                route.erase(route.begin() + i);
                found_pickup = true;
                break;
            }
        }
        if (!found_pickup) continue;

        // Remove the dropoff node (must be in the same route).
        for (int i = static_cast<int>(route.size()) - 1; i >= 0; --i) {
            if (static_cast<std::size_t>(route[i]) == dropoff_node) {
                route.erase(route.begin() + i);
                break;
            }
        }

        unassigned.push_back(req_id);
        auto eval = solution_.evaluate(instance_);
        current_benefit_ = eval.feasible ? eval.benefit : 0.0;
        return true;
    }
    return false;
}

double State::try_insert(int req_id, int taxi_id,
                          int pickup_pos, int dropoff_pos) const {
    std::size_t pickup_node, dropoff_node;
    get_request_nodes(req_id, pickup_node, dropoff_node);

    Solution temp_sol = solution_;
    auto& route       = temp_sol.routes[taxi_id];
    const int sz      = static_cast<int>(route.size());
    const int N       = static_cast<int>(instance_.passengers.size());

    if (req_id < N) {
        // Passenger: dropoff forced immediately after pickup.
        if (pickup_pos < 0 || pickup_pos > sz) return -1e18;
        route.insert(route.begin() + pickup_pos,     pickup_node);
        route.insert(route.begin() + pickup_pos + 1, dropoff_node);
    } else {
        // Parcel: insert pickup first (at pickup_pos in the ORIGINAL route),
        // then dropoff at dropoff_pos in the MODIFIED route (size sz+1).
        // This allows dp == sz+1 (append at end) and handles empty routes.
        if (pickup_pos < 0 || pickup_pos > sz ||
            dropoff_pos <= pickup_pos || dropoff_pos > sz + 1) return -1e18;
        route.insert(route.begin() + pickup_pos,  pickup_node);
        route.insert(route.begin() + dropoff_pos, dropoff_node);
    }

    const auto eval = temp_sol.evaluate(instance_);
    return eval.feasible ? eval.benefit : -1e18;
}

bool State::do_insert(int req_id, int taxi_id,
                       int pickup_pos, int dropoff_pos) {
    std::size_t pickup_node, dropoff_node;
    get_request_nodes(req_id, pickup_node, dropoff_node);

    auto& route  = solution_.routes[taxi_id];
    const int sz = static_cast<int>(route.size());
    const int N  = static_cast<int>(instance_.passengers.size());

    if (req_id < N) {
        if (pickup_pos < 0 || pickup_pos > sz) return false;
        route.insert(route.begin() + pickup_pos,     pickup_node);
        route.insert(route.begin() + pickup_pos + 1, dropoff_node);
    } else {
        // Same semantics as try_insert: pickup first, dropoff into modified route.
        if (pickup_pos < 0 || pickup_pos > sz ||
            dropoff_pos <= pickup_pos || dropoff_pos > sz + 1) return false;
        route.insert(route.begin() + pickup_pos,  pickup_node);
        route.insert(route.begin() + dropoff_pos, dropoff_node);
    }

    const auto eval = solution_.evaluate(instance_);
    if (eval.feasible) {
        current_benefit_ = eval.benefit;
        // Remove req_id from the unassigned list.
        auto it = std::find(unassigned.begin(), unassigned.end(), req_id);
        if (it != unassigned.end()) unassigned.erase(it);
        return true;
    }

    // Infeasible – roll back by removing both nodes that were just inserted.
    for (int i = static_cast<int>(route.size()) - 1; i >= 0; --i) {
        if (static_cast<std::size_t>(route[i]) == dropoff_node) {
            route.erase(route.begin() + i);
            break;
        }
    }
    for (int i = static_cast<int>(route.size()) - 1; i >= 0; --i) {
        if (static_cast<std::size_t>(route[i]) == pickup_node) {
            route.erase(route.begin() + i);
            break;
        }
    }
    return false;
}

} // namespace alns
} // namespace soict
