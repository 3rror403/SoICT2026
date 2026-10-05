#pragma once
#include "search/alns/operators/LocalSearch/Move.hpp"
#include "search/alns/operators/Repair/InsertionSearch.hpp"

namespace soict::alns {
class AddRequest final : public Move {
public:
    std::string_view name() const override { return "add-request"; }
    bool improve(SearchState& state) const override;
private:
    mutable PositiveInsertionCache cache_;
    mutable BestInsertionCache fallback_cache_;
    mutable std::vector<std::size_t> requests_;
};
} // namespace soict::alns
