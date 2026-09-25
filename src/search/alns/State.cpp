#include "search/alns/State.hpp"

namespace soict {
namespace alns {

State::State(const Instance& instance) : instance_(instance), current_benefit_(0.0) {
    solution_.routes.resize(instance.taxis.size());
}

State::State(const Instance& instance, const Solution& init_sol) 
    : instance_(instance), solution_(init_sol), current_benefit_(0.0) {
    auto eval = solution_.evaluate(instance);
    current_benefit_ = eval.feasible ? eval.benefit : 0.0;
}

double State::benefit() const {
    return current_benefit_;
}

bool State::is_feasible() const {
    auto eval = solution_.evaluate(instance_);
    return eval.feasible;
}

bool State::insert(int node_id, int taxi_id, int pos) {
    // TODO: implement logic to insert node efficiently
    return false;
}

bool State::remove(int node_id) {
    // TODO: implement logic to remove node efficiently
    return false;
}

} // namespace alns
} // namespace soict
