#pragma once
#include "search/alns/operators/LocalSearch/Move.hpp"
#include "search/alns/operators/LocalSearch/MoveCache.hpp"
#include "search/alns/FastRouteCheck.hpp"
#include "search/alns/operators/LocalSearch/RelocationPool.hpp"
#include <optional>

namespace soict::alns {
class ParcelRelocationContext {
public:
    ParcelRelocationContext(const SearchState& state, std::size_t request);
    [[nodiscard]] bool valid() const { return served_; }
    [[nodiscard]] bool removal_feasible() const { return removal_gain_.has_value(); }
    [[nodiscard]] double removal_savings() const { return removal_savings_; }
    [[nodiscard]] std::size_t length(std::size_t taxi) const;
    [[nodiscard]] std::optional<double> gain(std::size_t taxi,
        std::size_t pickup_boundary, std::size_t delivery_boundary) const;
    [[nodiscard]] std::optional<double> gain(std::size_t taxi, const RouteCache& route,
        std::size_t pickup_boundary, std::size_t delivery_boundary) const;
    // Benefit delta from changed travel-cost arcs; feasibility is checked by gain().
    [[nodiscard]] std::optional<double> calculate_delta_fast(std::size_t taxi,
        const RouteCache& route, std::size_t pickup_boundary,
        std::size_t delivery_boundary) const;
    [[nodiscard]] std::optional<FastRouteCheck> pickup_prefix(
        std::size_t taxi, const RouteCache& route, std::size_t pickup_boundary) const;
    [[nodiscard]] std::optional<double> gain_from_prefix(std::size_t taxi,
        const RouteCache& route, std::size_t pickup_boundary,
        std::size_t delivery_boundary, const FastRouteCheck& prefix) const;
private:
    const SearchState& state_;
    std::size_t request_, source_, pickup_at_, delivery_at_;
    bool served_ = false;
    std::optional<double> removal_gain_;
    double removal_savings_ = 0;
    Segment pickup_, delivery_;
    [[nodiscard]] std::size_t original_position(std::size_t position) const;
    [[nodiscard]] bool source_boundary_allowed(const RouteCache& route,
                                                std::size_t boundary) const;
    void add_source_range(FastRouteCheck& candidate, std::size_t first,
                          std::size_t end) const;
};

class ParcelRelocate final : public Move {
public:
    std::string_view name() const override { return "parcel_relocate"; }
    bool improve(SearchState&) const override;
private:
    mutable MoveCache cache_;
    [[nodiscard]] std::optional<InsertionChoice> best_on_taxi(
        const SearchState&, const ParcelRelocationContext&, std::size_t request,
        std::size_t taxi, const RouteCache&) const;
};
} // namespace soict::alns
