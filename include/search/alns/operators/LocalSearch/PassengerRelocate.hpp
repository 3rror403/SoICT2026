#pragma once
#include "search/alns/operators/LocalSearch/Move.hpp"
#include "search/alns/operators/LocalSearch/MoveCache.hpp"
#include "search/alns/operators/LocalSearch/RelocationSource.hpp"
#include <optional>

namespace soict::alns {
// One source-removal context is shared by every candidate destination. For a
// same-route move, positions refer to the route after the passenger is removed.
class PassengerRelocationContext : public RelocationSource<true> {
public:
    using RelocationSource<true>::RelocationSource;
    [[nodiscard]] std::optional<double> gain(std::size_t taxi,
        const RouteCache& route, std::size_t boundary) const;
    // Benefit delta from changed travel-cost arcs; feasibility is checked by gain().
    [[nodiscard]] std::optional<double> calculate_delta_fast(std::size_t taxi,
        const RouteCache& route, std::size_t boundary) const;
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
