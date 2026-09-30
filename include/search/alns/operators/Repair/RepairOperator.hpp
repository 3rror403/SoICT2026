#pragma once
#include "search/alns/operators/Destroy/DestroyOperator.hpp"
#include <span>

namespace soict::alns {
class RepairOperator {
public:
    virtual ~RepairOperator() = default;
    virtual std::string_view name() const = 0;
    virtual void apply(SearchState&, std::span<const std::size_t> removed,
                       std::mt19937_64&) const = 0;
};
} // namespace soict::alns
