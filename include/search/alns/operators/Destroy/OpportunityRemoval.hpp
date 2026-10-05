#pragma once
#include "search/alns/operators/Destroy/DestroyOperator.hpp"

namespace soict::alns {
// Open space around an unserved, individually profitable request. Related
// removal normally seeds from a served pickup and never targets lost revenue.
class OpportunityRemoval final : public DestroyOperator {
public:
    std::string_view name() const override { return "opportunity"; }
    std::vector<std::size_t> apply(SearchState&, std::size_t, std::mt19937_64&) const override;
};
} // namespace soict::alns
