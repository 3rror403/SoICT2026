#pragma once
#include <cstddef>
#include <random>
#include <vector>

namespace soict::alns {
struct Choice {
    std::size_t couple = 0, size_index = 0, removal_count = 0;
};
class Balance {
public:
    Balance(std::size_t couples, std::size_t requests, bool thompson);
    Choice select(std::mt19937_64& rng, std::size_t served);
    void update(Choice choice, double reward);
    [[nodiscard]] const std::vector<double>& rewards() const { return reward_sum_; }
    [[nodiscard]] const std::vector<std::size_t>& calls() const { return calls_; }
private:
    struct Arm {
        double weight = 1, sum = 0, square_sum = 0;
        std::size_t count = 0;
    };
    bool thompson_;
    std::vector<Arm> couples_;
    std::vector<std::vector<Arm>> sizes_;
    std::vector<std::size_t> powers_;
    std::vector<double> reward_sum_;
    std::vector<std::size_t> calls_;
    std::size_t choose(const std::vector<Arm>& arms, std::mt19937_64& rng) const;
};
} // namespace soict::alns
