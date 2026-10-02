#pragma once

#include "search/alns/operators/LocalSearch/Move.hpp"

namespace soict::alns {
// Exchange suffixes at boundaries where every request is wholly on one side.
class CleanCut2OptStar final : public Move {
public:
    std::string_view name() const override { return "clean-cut-2opt-star"; }
    bool improve(SearchState& state) const override;
    [[nodiscard]] std::size_t calls() const { return calls_; }
    [[nodiscard]] std::size_t applied_moves() const { return applied_moves_; }
    [[nodiscard]] double total_gain() const { return total_gain_; }
private:
    mutable std::size_t calls_ = 0, applied_moves_ = 0;
    mutable double total_gain_ = 0;
};
} // namespace soict::alns
