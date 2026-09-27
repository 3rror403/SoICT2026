#include "search/alns/ALNSSolver.hpp"
#include "search/alns/State.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <random>

namespace soict {
namespace alns {

// ---- Construction ----

ALNSSolver::ALNSSolver(const Instance& instance, const ALNSConfig& config)
    : instance_(instance),
      config_(config),
      rng_(std::random_device{}()) {}

void ALNSSolver::add_destroy_operator(std::unique_ptr<DestroyOperator> op) {
    destroy_ops_.push_back(std::move(op));
    destroy_weights_.push_back(1.0);
}

void ALNSSolver::add_repair_operator(std::unique_ptr<RepairOperator> op) {
    repair_ops_.push_back(std::move(op));
    repair_weights_.push_back(1.0);
}

// ---- Roulette-wheel selection ----

int ALNSSolver::select_operator(const std::vector<double>& weights) {
    double total = 0.0;
    for (double w : weights) total += w;

    std::uniform_real_distribution<double> dis(0.0, total);
    double val = dis(rng_);

    for (std::size_t i = 0; i < weights.size(); ++i) {
        val -= weights[i];
        if (val <= 0.0) return static_cast<int>(i);
    }
    return static_cast<int>(weights.size()) - 1;
}

// ---- Main loop ----

ALNSResult ALNSSolver::solve() {
    if (destroy_ops_.empty() || repair_ops_.empty()) return {};

    const auto wall_start = std::chrono::steady_clock::now();
    auto elapsed = [&]() -> double {
        return std::chrono::duration<double>(
            std::chrono::steady_clock::now() - wall_start).count();
    };

    // ---- Build initial solution with first repair operator ----
    State current_state(instance_);
    current_state.init_all_unassigned();
    repair_ops_[0]->repair(current_state);

    State best_state = current_state;
    double temperature = config_.start_temperature;

    std::uniform_int_distribution<int> remove_dist(config_.q_min, config_.q_max);
    std::uniform_real_distribution<double> uni01(0.0, 1.0);

    ALNSResult result;

    for (int iter = 0; iter < config_.max_iterations; ++iter) {
        if (elapsed() >= config_.time_limit) break;

        // ---- Select operators ----
        const int d_idx = select_operator(destroy_weights_);
        const int r_idx = select_operator(repair_weights_);

        // ---- Apply destroy + repair to a copy ----
        State new_state = current_state;
        destroy_ops_[d_idx]->destroy(new_state, remove_dist(rng_));
        repair_ops_[r_idx]->repair(new_state);

        // ---- Simulated-annealing acceptance ----
        const double new_benefit = new_state.benefit();
        const double delta       = new_benefit - current_state.benefit();
        const bool is_new_best   = (new_benefit > best_state.benefit());

        const bool accept = (delta >= 0.0) ||
            (uni01(rng_) < std::exp(delta / std::max(temperature, 1e-10)));

        // ---- Compute weight-update score ----
        double score = 0.0;
        if (is_new_best)        score = config_.score_new_best;
        else if (accept && delta > 0.0) score = config_.score_better;
        else if (accept)        score = config_.score_accepted;

        // ---- Update solution and best ----
        if (accept)      current_state = new_state;
        if (is_new_best) best_state    = new_state;

        // ---- Adaptive weight update ----
        if (score > 0.0) {
            const double lam = config_.decay_factor;
            destroy_weights_[d_idx] =
                (1.0 - lam) * destroy_weights_[d_idx] + lam * score;
            repair_weights_[r_idx]  =
                (1.0 - lam) * repair_weights_[r_idx]  + lam * score;
        }

        // ---- Cool down ----
        temperature *= config_.cooling_rate;
        ++result.iterations;
    }

    result.solution = best_state.get_solution();
    result.benefit  = best_state.benefit();
    result.seconds  = elapsed();
    return result;
}

// ---- Convenience wrapper ----

ALNSResult solve_alns(const Instance& instance, double time_limit) {
    ALNSConfig config;
    config.time_limit = time_limit;

    ALNSSolver solver(instance, config);

    // Destroy operators (order matters for weight initialisation).
    solver.add_destroy_operator(std::make_unique<RandomRemoval>());
    solver.add_destroy_operator(std::make_unique<WorstRemoval>());

    // Repair operators.
    solver.add_repair_operator(std::make_unique<GreedyInsertion>());
    solver.add_repair_operator(std::make_unique<RandomInsertion>());

    return solver.solve();
}

} // namespace alns
} // namespace soict
