#pragma once
#include "search/alns/operators/LocalSearch/Move.hpp"

namespace soict::alns {
class ReplaceRequest final : public Move {
public:
    std::string_view name() const override { return "replace-request"; }
    bool improve(SearchState& state) const override;
};
} // namespace soict::alns
