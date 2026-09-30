#pragma once
#include "search/alns/operators/Destroy/DestroyOperator.hpp"

namespace soict::alns {
class RouteRemoval final : public DestroyOperator {
public:
    std::string_view name() const override { return "route-removal"; }
    std::vector<std::size_t> apply(SearchState& state, std::size_t count,
                                   std::mt19937_64& rng) const override;
};
} // namespace soict::alns
