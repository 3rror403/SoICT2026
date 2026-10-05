#pragma once

#include "search/alns/operators/LocalSearch/Move.hpp"
#include <optional>
#include <queue>

namespace soict::alns {
struct RequestSwapChoice {
    std::size_t first, second;
    double gain;
};

// Both endpoints move together; incoming passenger delivery follows pickup.
// Returns no value for an invalid pair or infeasible completed routes.
std::optional<double> request_swap_gain(const SearchState&, std::size_t first,
                                       std::size_t second);

class SwapRequest final : public Move {
public:
    explicit SwapRequest(bool use_smd = true) : use_smd_(use_smd) {}
    std::string_view name() const override { return "swap-request"; }
    bool improve(SearchState&) const override;
    [[nodiscard]] std::optional<RequestSwapChoice> best_choice(const SearchState&) const;
private:
    struct Descriptor {
        std::uint64_t generation = 0;
        std::optional<RequestSwapChoice> choice;
    };
    struct QueuedChoice {
        RequestSwapChoice choice;
        std::size_t taxi_a, taxi_b;
        std::uint64_t version_a, version_b, generation;
    };
    struct LowerPriority {
        bool operator()(const QueuedChoice&, const QueuedChoice&) const;
    };
    using Queue = std::priority_queue<QueuedChoice, std::vector<QueuedChoice>, LowerPriority>;
    bool use_smd_;
    mutable const Instance* instance_ = nullptr;
    mutable std::vector<std::uint64_t> versions_;
    mutable std::vector<std::vector<Descriptor>> descriptors_; // taxi_a < taxi_b
    mutable Queue queue_;
    void synchronize(const SearchState&) const;
    void enqueue(std::size_t taxi_a, std::size_t taxi_b) const;
};
} // namespace soict::alns
