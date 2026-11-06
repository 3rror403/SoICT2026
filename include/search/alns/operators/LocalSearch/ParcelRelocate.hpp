#pragma once
#include "search/alns/operators/LocalSearch/Move.hpp"
#include "search/alns/operators/LocalSearch/MoveCache.hpp"
#include "search/alns/operators/LocalSearch/RelocationSource.hpp"
#include <optional>

namespace soict::alns {
class ParcelRelocationContext : public RelocationSource<false> {
public:
    ParcelRelocationContext(const SearchState& state, std::size_t request);
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
    Segment pickup_, delivery_;
};

class ParcelRelocate final : public Move {
public:
    std::string_view name() const override { return "parcel_relocate"; }
    bool improve(SearchState&) const override;
private:
    mutable MoveCache cache_;
    [[nodiscard]] std::optional<InsertionChoice> best_on_taxi(
        const SearchState&, const ParcelRelocationContext&, std::size_t request,
        std::size_t taxi, const RouteCache&, std::uint64_t source_version) const;
};
} // namespace soict::alns
