#pragma once
#include "search/alns/operators/Repair/RepairOperator.hpp"
#include "search/alns/operators/Repair/InsertionSearch.hpp"

namespace soict::alns {
// Repeatedly select the largest positive marginal benefit across the pool.
class ProfitInsertion final : public RepairOperator {
public:
    std::string_view name() const override { return "profit"; }
    void apply(SearchState&, std::span<const std::size_t>, std::mt19937_64&) const override;
private:
    mutable PositiveInsertionCache positive_cache_;
    mutable BestInsertionCache fallback_cache_;
};
} // namespace soict::alns
