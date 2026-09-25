#pragma once
#include "model/Instance.hpp"
#include "model/Solution.hpp"
#include "search/alns/Operators.hpp"
#include <memory>
#include <vector>

namespace soict {
namespace alns {

struct ALNSConfig {
    double time_limit = 300.0;
    int max_iterations = 10000;
    
    // SA parameters
    double start_temperature = 100.0;
    double cooling_rate = 0.9995;
    
    // Weight update scores
    double score_new_best = 33.0;
    double score_better = 9.0;
    double score_accepted = 13.0;
    double decay_factor = 0.1; // rate of adapting weights
};

struct ALNSResult {
    Solution solution;
    double benefit = 0.0;
    double seconds = 0.0;
    int iterations = 0;
};

class ALNSSolver {
public:
    ALNSSolver(const Instance& instance, const ALNSConfig& config = ALNSConfig());

    void add_destroy_operator(std::unique_ptr<DestroyOperator> op);
    void add_repair_operator(std::unique_ptr<RepairOperator> op);

    ALNSResult solve();

private:
    const Instance& instance_;
    ALNSConfig config_;

    std::vector<std::unique_ptr<DestroyOperator>> destroy_ops_;
    std::vector<std::unique_ptr<RepairOperator>> repair_ops_;
    
    std::vector<double> destroy_weights_;
    std::vector<double> repair_weights_;
    
    int select_operator(const std::vector<double>& weights);
};

ALNSResult solve_alns(const Instance& instance, double time_limit);

} // namespace alns
} // namespace soict
