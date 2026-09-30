#pragma once
#include "search/alns/operators/LocalSearch/Move.hpp"
#include <cstdint>
#include <limits>
#include <vector>

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

    mutable std::vector<std::vector<TopK>> cache_;
    mutable std::vector<std::uint64_t> versions_;

    void update_cache(const SearchState& state, std::size_t taxi) const;
};

} // namespace soict::alns
