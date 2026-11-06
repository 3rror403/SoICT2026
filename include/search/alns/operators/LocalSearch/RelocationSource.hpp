#pragma once

#include "search/alns/FastRouteCheck.hpp"
#include "search/alns/MoveCost.hpp"
#include "search/alns/SearchState.hpp"

namespace soict::alns {

// Shared source-route data. Passenger and parcel contexts keep their own
// destination scans, but removal costs and virtual source positions coincide.
template<bool Passenger>
class RelocationSource {
public:
    RelocationSource(const SearchState& state, std::size_t request)
        : state_(state), request_(request) {
        if constexpr (Passenger) {
            if (request >= state.passenger_count()) return;
        } else {
            if (request < state.passenger_count() || request >= state.request_count()) return;
        }
        const auto& loc = state.location(request);
        if (loc.taxi < 0) return;
        source_ = static_cast<std::size_t>(loc.taxi);
        pickup_at_ = loc.pickup;
        delivery_at_ = loc.dropoff;
        removal_gain_ = state.removal_gain(request);
        removal_savings_ = removal_savings_cost(state.instance(), source_,
            state.cache(source_).nodes(), pickup_at_, delivery_at_);
    }
    [[nodiscard]] bool valid() const { return source_ != std::numeric_limits<std::size_t>::max(); }
    [[nodiscard]] bool removal_feasible() const { return removal_gain_.has_value(); }
    [[nodiscard]] double removal_savings() const { return removal_savings_; }

protected:
    const SearchState& state_;
    std::size_t request_, source_ = std::numeric_limits<std::size_t>::max();
    std::size_t pickup_at_ = 0, delivery_at_ = 0;
    std::optional<double> removal_gain_;
    double removal_savings_ = 0;

    [[nodiscard]] std::size_t original_position(std::size_t position) const {
        if (position < pickup_at_) return position;
        if (position < delivery_at_ - 1) return position + 1;
        return position + 2;
    }
    [[nodiscard]] bool source_boundary_allowed(const RouteCache& route, std::size_t boundary) const {
        return !boundary || route.boundary_allowed(state_.instance(), original_position(boundary - 1) + 1);
    }
    void add_source_range(FastRouteCheck& candidate, std::size_t first, std::size_t end) const {
        const auto left_end = std::min(end, pickup_at_);
        if (first < left_end) candidate.range(first, left_end);
        const auto middle_start = std::max(first, pickup_at_);
        const auto middle_end = std::min(end, delivery_at_ - 1);
        if (middle_start < middle_end) candidate.range(middle_start + 1, middle_end + 1);
        const auto right_start = std::max(first, delivery_at_ - 1);
        if (right_start < end) candidate.range(right_start + 2, end + 2);
    }
};

} // namespace soict::alns
