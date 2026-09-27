#pragma once
#include "model/Instance.hpp"
#include "model/Solution.hpp"
#include <vector>

namespace soict {
namespace alns {

// State wraps the current Solution during ALNS search.
// unassigned holds request indices: [0, N) = passengers, [N, N+M) = parcels.
class State {
public:
    explicit State(const Instance& instance);
    State(const Instance& instance, const Solution& init_sol);

    // Copy constructor & assignment: instance_ (a reference) is not
    // re-seated; all State copies in the ALNS loop share the same instance.
    State(const State& other)
        : instance_(other.instance_),
          solution_(other.solution_),
          current_benefit_(other.current_benefit_),
          unassigned(other.unassigned) {}

    State& operator=(const State& other) {
        if (this != &other) {
            solution_         = other.solution_;
            current_benefit_  = other.current_benefit_;
            unassigned        = other.unassigned;
        }
        return *this;
    }

    // ---- Accessors ----
    double benefit() const;
    bool   is_feasible() const;

    const Instance& get_instance() const { return instance_; }
    const Solution& get_solution() const { return solution_; }

    // ---- Initialisation ----
    // Clear all routes and populate unassigned with every request [0, N+M).
    void init_all_unassigned();

    // ---- Mutation helpers ----
    // Scan routes and return every request index currently assigned to a taxi.
    std::vector<int> get_assigned_requests() const;

    // Remove both nodes of req_id from whichever route contains them,
    // append req_id to unassigned, and recompute current_benefit_.
    // Returns false if req_id is not found in any route.
    bool remove_request(int req_id);

    // Test inserting req_id into taxi_id at (pickup_pos, dropoff_pos) on a
    // Solution copy.  For passengers dropoff_pos is ignored (forced to
    // pickup_pos + 1).  Returns the new benefit, or -1e18 if infeasible.
    double try_insert(int req_id, int taxi_id,
                      int pickup_pos, int dropoff_pos) const;

    // Actually insert req_id into taxi_id.  On success removes req_id from
    // unassigned, updates current_benefit_, and returns true.
    // Rolls back and returns false if the resulting solution is infeasible.
    bool do_insert(int req_id, int taxi_id,
                   int pickup_pos, int dropoff_pos);

    // Unrouted request indices (passengers and parcels).
    std::vector<int> unassigned;

private:
    const Instance& instance_;
    Solution        solution_;
    double          current_benefit_;

    // Fill pickup_node / dropoff_node for the given request index.
    void get_request_nodes(int req_id,
                           std::size_t& pickup_node,
                           std::size_t& dropoff_node) const;
};

} // namespace alns
} // namespace soict
