#include "search/alns/ALNSSolver.hpp"
#include "search/alns/State.hpp"
#include <chrono>
#include <random>
#include <cmath>

namespace soict {
namespace alns {

ALNSSolver::ALNSSolver(const Instance& instance, const ALNSConfig& config)
    : instance_(instance), config_(config) {}

void ALNSSolver::add_destroy_operator(std::unique_ptr<DestroyOperator> op) {
    destroy_ops_.push_back(std::move(op));
    destroy_weights_.push_back(1.0);
}

void ALNSSolver::add_repair_operator(std::unique_ptr<RepairOperator> op) {
    repair_ops_.push_back(std::move(op));
    repair_weights_.push_back(1.0);
}

int ALNSSolver::select_operator(const std::vector<double>& weights) {
    double total = 0.0;
    for (double w : weights) total += w;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, total);
    double val = dis(gen);
    
    for (size_t i = 0; i < weights.size(); ++i) {
        val -= weights[i];
        if (val <= 0.0) return i;
    }
    return weights.size() - 1;
}

ALNSResult ALNSSolver::solve() {
    auto start_time = std::chrono::steady_clock::now();
    ALNSResult result;
    
    State current_state(instance_);
    State best_state(instance_);
    
    double temperature = config_.start_temperature;
    
    for (int iter = 0; iter < config_.max_iterations; ++iter) {
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = now - start_time;
        if (elapsed.count() > config_.time_limit) {
            break;
        }
        
        State new_state = current_state;
        
        if (!destroy_ops_.empty() && !repair_ops_.empty()) {
            int d_idx = select_operator(destroy_weights_);
            int r_idx = select_operator(repair_weights_);
            
            // Random number to remove
            int remove_cnt = 2; // placeholder
            destroy_ops_[d_idx]->destroy(new_state, remove_cnt);
            repair_ops_[r_idx]->repair(new_state);
            
            // SA acceptance
            double delta = new_state.benefit() - current_state.benefit();
            bool accept = false;
            
            if (delta >= 0) { // Assuming maximization
                accept = true;
            } else {
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_real_distribution<> dis(0.0, 1.0);
                if (dis(gen) < std::exp(delta / temperature)) {
                    accept = true;
                }
            }
            
            if (accept) {
                current_state = new_state;
                if (current_state.benefit() > best_state.benefit()) {
                    best_state = current_state;
                }
            }
        }
        
        temperature *= config_.cooling_rate;
        result.iterations++;
    }
    
    auto end_time = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    
    result.solution = best_state.get_solution();
    result.benefit = best_state.benefit();
    result.seconds = elapsed.count();
    
    return result;
}

ALNSResult solve_alns(const Instance& instance, double time_limit) {
    ALNSConfig config;
    config.time_limit = time_limit;
    
    ALNSSolver solver(instance, config);
    solver.add_destroy_operator(std::make_unique<RandomRemoval>());
    solver.add_repair_operator(std::make_unique<GreedyInsertion>());
    
    return solver.solve();
}

} // namespace alns
} // namespace soict
