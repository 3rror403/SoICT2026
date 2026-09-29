#pragma once
#include "search/alns/SearchState.hpp"
#include <cstddef>
#include <random>
#include <string_view>
#include <vector>

namespace soict::alns {
class DestroyOperator {
public:
    virtual ~DestroyOperator() = default;
    virtual std::string_view name() const = 0;
    virtual std::vector<std::size_t> apply(SearchState& state, std::size_t count,
                                           std::mt19937_64& rng) const = 0;
};
} // namespace soict::alns
