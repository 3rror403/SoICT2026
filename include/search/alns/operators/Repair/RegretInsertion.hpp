#pragma once
#include "search/alns/operators/Repair/RepairOperator.hpp"

namespace soict::alns {

// V0 (current): regret = best - second (if second finite), else best.
// When only 1 feasible position: second = -inf → regret = best.
// This coincidentally equals best - max(second, 0) only for the 1-option case.
class RegretInsertionV0 final : public RepairOperator {
public:
    std::string_view name() const override { return "regret_v0"; }
    void apply(SearchState&, std::span<const std::size_t>, std::mt19937_64&) const override;
};

// V1: regret = best - max(second, 0).
// Treats “not serving” (gain=0) as the alternative when second < 0.
// Differs from V0 when there is a second feasible position with gain < 0.
class RegretInsertionV1 final : public RepairOperator {
public:
    std::string_view name() const override { return "regret_v1"; }
    void apply(SearchState&, std::span<const std::size_t>, std::mt19937_64&) const override;
};

// V2: regret = +inf when only 1 feasible position (absolute priority),
//      else best - second. Tie-break by best->gain descending.
class RegretInsertionV2 final : public RepairOperator {
public:
    std::string_view name() const override { return "regret_v2"; }
    void apply(SearchState&, std::span<const std::size_t>, std::mt19937_64&) const override;
};

// Legacy alias kept for backward compatibility.
using RegretInsertion = RegretInsertionV0;

} // namespace soict::alns
