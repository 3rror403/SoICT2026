#pragma once
#include "search/alns/operators/Destroy/DestroyOperator.hpp"

namespace soict::alns {
class Move {
public:
    virtual ~Move() = default;
    virtual std::string_view name() const = 0;
    virtual bool improve(SearchState& state) const = 0;
};
} // namespace soict::alns
