#include "search/alns/Balance.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace soict::alns {
Balance::Balance(std::size_t couples, std::size_t requests, bool thompson)
    : thompson_(thompson), couples_(couples), sizes_(couples),
      reward_sum_(couples, 0), calls_(couples, 0) {
    if (couples == 0) throw std::invalid_argument("BALANCE needs an operator couple");
    for (std::size_t p = 1; p <= requests; p *= 2) {
        powers_.push_back(p);
        if (p > requests / 2) break;
    }
    if (powers_.empty()) powers_.push_back(1);
    for (auto& arms : sizes_) arms.resize(powers_.size());
}

std::size_t Balance::choose(const std::vector<Arm>& arms, std::mt19937_64& rng) const {
    if (!thompson_) {
        double total = 0;
        for (const auto& arm : arms) total += arm.weight;
        std::uniform_real_distribution<double> draw(0, total);
        double target = draw(rng);
        for (std::size_t i = 0; i < arms.size(); ++i) {
            target -= arms[i].weight;
            if (target <= 0) return i;
        }
        return arms.size() - 1;
    }
    // Normal-Gamma posterior with MV-FSTSP priors.
    constexpr double mu0 = 0, lambda0 = 0.01, alpha0 = 1, beta0 = 100;
    std::size_t choice = 0;
    double best = -std::numeric_limits<double>::infinity();
    for (std::size_t i = 0; i < arms.size(); ++i) {
        const auto& a = arms[i];
        const double count = static_cast<double>(a.count);
        const double lambda = lambda0 + count;
        const double mean = (lambda0 * mu0 + a.sum) / lambda;
        const double alpha = alpha0 + count / 2;
        const double beta = beta0 + 0.5 *
            (a.square_sum + lambda0 * mu0 * mu0 - lambda * mean * mean);
        std::gamma_distribution<double> gamma(alpha, 1.0 / std::max(beta, 1e-12));
        const double precision = gamma(rng);
        std::normal_distribution<double> normal(mean,
            1.0 / std::sqrt(std::max(lambda * precision, 1e-12)));
        const double sample = normal(rng);
        if (sample > best) { best = sample; choice = i; }
    }
    return choice;
}

Choice Balance::select(std::mt19937_64& rng, std::size_t served) {
    Choice choice;
    choice.couple = choose(couples_, rng);
    choice.size_index = choose(sizes_[choice.couple], rng);
    choice.removal_count = std::min(served, powers_[choice.size_index]);
    return choice;
}

void Balance::update(Choice choice, double reward) {
    reward = std::max(0.0, reward);
    auto update_arm = [reward](Arm& a) {
        ++a.count;
        a.sum += reward;
        a.square_sum += reward * reward;
        a.weight = 0.9 * a.weight + 0.1 * (0.01 + reward);
    };
    update_arm(couples_[choice.couple]);
    update_arm(sizes_[choice.couple][choice.size_index]);
    reward_sum_[choice.couple] += reward;
    ++calls_[choice.couple];
}
} // namespace soict::alns
