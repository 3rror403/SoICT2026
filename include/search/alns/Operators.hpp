#pragma once
#include "search/alns/State.hpp"
#include <string>
#include <vector>

namespace soict {
namespace alns {

// ---- Abstract interfaces ----

class DestroyOperator {
public:
    virtual ~DestroyOperator() = default;
    virtual std::string name() const = 0;
    virtual void destroy(State& state, int num_to_remove) = 0;
};

class RepairOperator {
public:
    virtual ~RepairOperator() = default;
    virtual std::string name() const = 0;
    virtual void repair(State& state) = 0;
};

// ---- Destroy operators ----

// Removes num_to_remove randomly chosen assigned requests.
class RandomRemoval : public DestroyOperator {
public:
    std::string name() const override { return "RandomRemoval"; }
    void destroy(State& state, int num_to_remove) override;
};

// Removes the num_to_remove requests that contribute the least to the
// current objective (marginal-contribution ordering).
class WorstRemoval : public DestroyOperator {
public:
    std::string name() const override { return "WorstRemoval"; }
    void destroy(State& state, int num_to_remove) override;
};

// ---- Repair operators ----

// For each unassigned request (in order), finds and applies the single best
// feasible insertion across all taxis and positions.
class GreedyInsertion : public RepairOperator {
public:
    std::string name() const override { return "GreedyInsertion"; }
    void repair(State& state) override;
};

// For each unassigned request (shuffled), picks a random feasible insertion.
class RandomInsertion : public RepairOperator {
public:
    std::string name() const override { return "RandomInsertion"; }
    void repair(State& state) override;
};

} // namespace alns
} // namespace soict
