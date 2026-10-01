#pragma once

#include "search/alns/RouteCache.hpp"
#include <algorithm>
#include <optional>

namespace soict::alns {
// Fixed-size feasibility accumulator for a move's unchanged route pieces.
// A piece maps arrival x to departure max(x + duration, ready). Joining pieces
// keeps that map, the latest admissible input, and the parcel-load extrema.
// Travel cost is deliberately absent: each move computes it from changed arcs.
class FastRouteCheck {
public:
    FastRouteCheck(const Instance& in, std::size_t taxi, const RouteCache& route)
        : in_(in), taxi_(taxi), route_(route) {}

    void range(std::size_t first, std::size_t end) {
        if (first < end) append(route_.range(in_, first, end - 1));
    }
    void node(std::size_t id) { append(single_node(in_, id)); }
    void segment(const Segment& next) { append(next); }

    [[nodiscard]] bool feasible() const {
        if (!summary_) return true;
        const auto& s = *summary_;
        return s.feasible && in_.time(taxi_, s.first) <= s.latest_input &&
               s.min_load >= -1e-8 &&
               s.max_load <= in_.taxis[taxi_].parcel_capacity + 1e-8;
    }

    [[nodiscard]] std::optional<double> duration() const {
        if (!summary_) return 0.0;
        return summary_->duration;
    }

    [[nodiscard]] std::optional<double> departure() const {
        if (!summary_) return 0.0;
        const double travel_to_start = in_.time(taxi_, summary_->first);
        return std::max(travel_to_start + summary_->duration, summary_->ready);
    }

private:
    void append(const Segment& next) {
        if (!summary_) {
            summary_ = next;
            return;
        }
        auto& s = *summary_;
        const double travel = in_.time(s.last, next.first);
        const double old_change = s.load_change;
        const double old_duration = s.duration;
        const double old_ready = s.ready;
        s.feasible = s.feasible && next.feasible &&
                     old_ready + travel <= next.latest_input;
        s.latest_input = std::min(s.latest_input,
                                  next.latest_input - travel - old_duration);
        s.duration = old_duration + travel + next.duration;
        s.ready = std::max(old_ready + travel + next.duration, next.ready);
        s.min_load = std::min(s.min_load, old_change + next.min_load);
        s.max_load = std::max(s.max_load, old_change + next.max_load);
        s.load_change = old_change + next.load_change;
        s.last = next.last;
    }

    const Instance& in_;
    std::size_t taxi_;
    const RouteCache& route_;
    std::optional<Segment> summary_;
};
} // namespace soict::alns
