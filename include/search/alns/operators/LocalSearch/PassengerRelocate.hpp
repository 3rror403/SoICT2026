#pragma once
#include "search/alns/operators/LocalSearch/Move.hpp"
#include "search/alns/operators/LocalSearch/MoveCache.hpp"
#include "search/alns/FastRouteCheck.hpp"
#include "search/alns/operators/LocalSearch/RelocationPool.hpp"
#include <optional>

namespace soict::alns {
// One source-removal context is shared by every candidate destination. For a
// same-route move, positions refer to the route after the passenger is removed.
class PassengerRelocationContext {
public:
    PassengerRelocationContext(const SearchState& state, std::size_t request);
    [[nodiscard]] bool valid() const { return served_; }
    [[nodiscard]] bool removal_feasible() const { return removal_gain_.has_value(); }
    [[nodiscard]] double removal_savings() const { return removal_savings_; }
    [[nodiscard]] std::optional<double> gain(std::size_t taxi,
        const RouteCache& route, std::size_t boundary) const;
    // Benefit delta from changed travel-cost arcs; feasibility is checked by gain().
    [[nodiscard]] std::optional<double> calculate_delta_fast(std::size_t taxi,
        const RouteCache& route, std::size_t boundary) const;
private:
    const SearchState& state_;
    std::size_t request_, source_, removed_at_;
    bool served_ = false;
    std::optional<double> removal_gain_;
    double removal_savings_ = 0;
    [[nodiscard]] std::size_t original_position(std::size_t position) const;
    void add_source_range(FastRouteCheck& candidate, std::size_t first,
                          std::size_t end) const;
};

class PassengerRelocate final : public Move {
public:
    std::string_view name() const override { return "passenger_relocate"; }
    bool improve(SearchState&) const override;
private:
    mutable MoveCache cache_;
    [[nodiscard]] std::optional<InsertionChoice> best_on_taxi(
        const SearchState&, const PassengerRelocationContext&, std::size_t request,
        std::size_t taxi, const RouteCache&, std::uint64_t source_version) const;
};
} // namespace soict::alns
