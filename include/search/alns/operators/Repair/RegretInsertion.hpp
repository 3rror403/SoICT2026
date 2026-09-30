#pragma once
#include "search/alns/operators/Repair/RepairOperator.hpp"

namespace soict::alns {
class RegretInsertion final : public RepairOperator {
public:
    std::string_view name() const override { return "regret2"; }
    void apply(SearchState&, std::span<const std::size_t>, std::mt19937_64&) const override;
};
} // namespace soict::alns
