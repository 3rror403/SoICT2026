#pragma once

#include "model/Instance.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <vector>

namespace soict::alns {

// A route segment maps arrival at its first node x to departure at its last
// node max(x + duration, ready). It is feasible only for x <= latest_input.
struct Segment {
    std::size_t first = 0, last = 0;
    double duration = 0;
    double ready = -std::numeric_limits<double>::infinity();
    double latest_input = std::numeric_limits<double>::infinity();
    double cost = 0;
    double load_change = 0, min_load = 0, max_load = 0;
    bool feasible = true;
};

Segment single_node(const Instance& instance, std::size_t node);
Segment join(const Instance& instance, const Segment& left, const Segment& right);

// One composition kernel for persistent route summaries and temporary checks.
// The cost-free specialization keeps feasibility scans free of cost accesses.
template<bool WithCost>
inline void append_segment(const Instance& in, Segment& s, const Segment& next) {
    const double travel = in.time(s.last, next.first);
    const double old_change = s.load_change, old_duration = s.duration, old_ready = s.ready;
    if constexpr (WithCost) s.cost = s.cost + in.cost(s.last, next.first) + next.cost;
    s.feasible = s.feasible && next.feasible && old_ready + travel <= next.latest_input;
    s.latest_input = std::min(s.latest_input, next.latest_input - travel - old_duration);
    s.duration = old_duration + travel + next.duration;
    s.ready = std::max(old_ready + travel + next.duration, next.ready);
    s.min_load = std::min(s.min_load, old_change + next.min_load);
    s.max_load = std::max(s.max_load, old_change + next.max_load);
    s.load_change = old_change + next.load_change;
    s.last = next.last;
}

class RouteCache {
public:
    RouteCache() = default;
    RouteCache(const Instance& instance, std::size_t taxi,
               const std::vector<std::size_t>& route);
    // Refill or copy logical contents while retaining allocated buffers.
    void rebuild(const Instance& instance, std::size_t taxi,
                 const std::vector<std::size_t>& route);
    void clear();
    void copy_from(const RouteCache& other);

    [[nodiscard]] Segment range(const Instance& instance, std::size_t first,
                                std::size_t last) const; // inclusive
    [[nodiscard]] bool boundary_allowed(const Instance& instance,
                                        std::size_t position) const;
    [[nodiscard]] double cost() const { return cost_; }
    [[nodiscard]] std::uint64_t version() const { return version_; }
    [[nodiscard]] const std::vector<std::size_t>& nodes() const { return nodes_; }
    [[nodiscard]] const std::vector<double>& arrival() const { return arrival_; }
    [[nodiscard]] const std::vector<double>& departure() const { return departure_; }
    [[nodiscard]] const std::vector<double>& load() const { return load_; }

private:
    std::vector<std::size_t> nodes_;
    std::vector<Segment> singles_;
    std::vector<std::vector<Segment>> table_;
    std::vector<double> arrival_, departure_, load_;
    double cost_ = 0;
    std::uint64_t version_ = 0;
};

// Builds a virtual route from a constant number of existing segments and new
// nodes. No route vector is copied while enumerating candidate positions.
template<bool WithCost>
class SegmentAccumulator {
public:
    SegmentAccumulator(const Instance& in, std::size_t taxi, const RouteCache& route)
        : in_(in), taxi_(taxi), route_(route) {}
    void range(std::size_t first, std::size_t end) {
        if constexpr (WithCost) { if (first == end) return; }
        else { if (first >= end) return; }
        segment(route_.range(in_, first, end - 1));
    }
    void node(std::size_t id) { segment(single_node(in_, id)); }
    void segment(const Segment& next) {
        if (summary_) append_segment<WithCost>(in_, *summary_, next);
        else summary_ = next;
    }
    // Keep the existing RouteCandidate interface at its call sites.
    void add_range(std::size_t first, std::size_t end) { range(first, end); }
    void add_segment(const Segment& next) { segment(next); }
    void add_node(std::size_t id) { node(id); }
    [[nodiscard]] bool feasible() const {
        if (!summary_) return true;
        const auto& s = *summary_;
        if constexpr (WithCost) {
            const double arrival = in_.time(taxi_, s.first);
            // Preserve RouteCandidate's original comparisons, including its
            // floating-point edge behavior; FastRouteCheck uses the <= form.
            return !(!s.feasible || arrival > s.latest_input || s.min_load < -1e-8 ||
                s.max_load > in_.taxis[taxi_].parcel_capacity + 1e-8);
        } else {
            return s.feasible && in_.time(taxi_, s.first) <= s.latest_input && s.min_load >= -1e-8 &&
                s.max_load <= in_.taxis[taxi_].parcel_capacity + 1e-8;
        }
    }
    [[nodiscard]] std::optional<double> cost_if_feasible() const {
        static_assert(WithCost);
        if (!feasible()) return std::nullopt;
        return summary_ ? in_.cost(taxi_, summary_->first) + summary_->cost : 0.0;
    }

private:
    const Instance& in_;
    std::size_t taxi_;
    const RouteCache& route_;
    std::optional<Segment> summary_;
};
using RouteCandidate = SegmentAccumulator<true>;
} // namespace soict::alns
