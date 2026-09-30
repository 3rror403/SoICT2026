#pragma once
#include "search/alns/operators/LocalSearch/Move.hpp"
#include <cstdint>
#include <limits>
#include <vector>
#include <algorithm>
#include <optional>

namespace soict::alns {

class SwapRequest final : public Move {
public:
    std::string_view name() const override { return "swap-request"; }
    bool improve(SearchState& state) const override;

private:
    struct Entry {
        std::size_t pickup = 0;
        std::size_t delivery = 0;
        double delta = std::numeric_limits<double>::infinity();
    };

    struct TopK {
        static constexpr std::size_t K = 3;
        Entry entries[K];
        std::size_t count = 0;

        void add(std::size_t p, std::size_t d, double delta) {
            if (count == K && delta >= entries[K - 1].delta - 1e-9) return;
            std::size_t i = count;
            while (i > 0 && entries[i - 1].delta > delta) {
                if (i < K) entries[i] = entries[i - 1];
                --i;
            }
            if (i < K) {
                entries[i] = {p, d, delta};
                if (count < K) ++count;
            }
        }
    };

    // Descriptor for a validated swap move
    struct SwapDescriptor {
        std::size_t r1, r2;           // requests to swap
        std::size_t t1, t2;           // their current taxis
        std::size_t p1, d1, p2, d2;   // insertion positions (r1→t2 at p1/d1, r2→t1 at p2/d2)
        double gain;
    };

    // Pool of swap descriptors with incremental invalidation
    struct SwapPool {
        void add(const SwapDescriptor& desc) {
            if (desc.gain > 1e-9) moves_.push_back(desc);
        }
        [[nodiscard]] std::optional<SwapDescriptor> best() const {
            if (moves_.empty()) return std::nullopt;
            return *std::max_element(moves_.begin(), moves_.end(),
                [](const auto& a, const auto& b) { return a.gain < b.gain; });
        }
        // Remove all descriptors involving requests on taxi t1 or t2
        void erase_affected(const SearchState& state, std::size_t t1, std::size_t t2) {
            std::erase_if(moves_, [&](const auto& m) {
                return m.t1 == t1 || m.t1 == t2 || m.t2 == t1 || m.t2 == t2;
            });
        }
        void clear() { moves_.clear(); }
        [[nodiscard]] bool empty() const { return moves_.empty(); }
    private:
        std::vector<SwapDescriptor> moves_;
    };

    mutable std::vector<std::vector<TopK>> cache_;
    mutable std::vector<std::uint64_t> versions_;

    void update_cache(const SearchState& state, std::size_t taxi) const;

    // Evaluate a single (r1, r2) pair and add valid descriptors to pool
    void evaluate_pair(const SearchState& state,
                       std::size_t r1, std::size_t r2,
                       double s1, double s2,
                       double best_gain,
                       SwapPool& pool) const;
};

} // namespace soict::alns
