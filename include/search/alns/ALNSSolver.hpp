#pragma once
#include "model/Instance.hpp"
#include "model/Solution.hpp"
#include "search/alns/Operators.hpp"
#include <memory>
#include <random>
#include <vector>

namespace soict {
namespace alns {

struct ALNSConfig {
    double time_limit     = 300.0;
    int    max_iterations = 10000;

    // Simulated-annealing parameters.
    double start_temperature = 100.0;
    double cooling_rate      = 0.9995;

    // Roulette-wheel weight-update scores.
    double score_new_best = 33.0;
    double score_better   =  9.0;
    double score_accepted = 13.0;

    // Weight decay: new_weight = (1-decay)*old + decay*score.
    double decay_factor = 0.1;

    // Destroy segment: remove between q_min and q_max requests per iteration.
    int q_min = 1;
    int q_max = 5;
};

struct ALNSResult {
    Solution solution;
    double   benefit    = 0.0;
    double   seconds    = 0.0;
    int      iterations = 0;
};

class ALNSSolver {
public:
    ALNSSolver(const Instance& instance,
               const ALNSConfig& config = ALNSConfig());

    void add_destroy_operator(std::unique_ptr<DestroyOperator> op);
    void add_repair_operator (std::unique_ptr<RepairOperator>  op);

    ALNSResult solve();

private:
    const Instance& instance_;
    ALNSConfig      config_;
    std::mt19937    rng_;

    std::vector<std::unique_ptr<DestroyOperator>> destroy_ops_;
    std::vector<std::unique_ptr<RepairOperator>>  repair_ops_;

    std::vector<double> destroy_weights_;
    std::vector<double> repair_weights_;

    int select_operator(const std::vector<double>& weights);
};

// Convenience wrapper: registers default operators and runs the solver.
ALNSResult solve_alns(const Instance& instance, double time_limit);

} // namespace alns
} // namespace soict
