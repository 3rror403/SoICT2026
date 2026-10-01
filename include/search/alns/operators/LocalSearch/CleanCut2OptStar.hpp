#pragma once
#include "search/alns/operators/LocalSearch/Move.hpp"

namespace soict::alns {
class CleanCut2OptStar final : public Move {
public:
    std::string_view name() const override { return "clean_cut_2opt"; }
    bool improve(SearchState& state) const override;
};
} // namespace soict::alns
