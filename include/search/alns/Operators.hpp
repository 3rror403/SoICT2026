#pragma once
#include "search/alns/State.hpp"
#include <string>
#include <vector>

namespace soict {
namespace alns {

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

// --- Basic Implementations ---

class RandomRemoval : public DestroyOperator {
public:
    std::string name() const override { return "RandomRemoval"; }
    void destroy(State& state, int num_to_remove) override;
};

class GreedyInsertion : public RepairOperator {
public:
    std::string name() const override { return "GreedyInsertion"; }
    void repair(State& state) override;
};

} // namespace alns
} // namespace soict
