#pragma once

#include "model/Instance.hpp"
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <vector>
#include "search/alns/Profiler.hpp"

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
class RouteCandidate {
public:
    RouteCandidate(const Instance& instance, std::size_t taxi,
                   const RouteCache& cache);
    void add_range(std::size_t first, std::size_t end); // [first, end)
    void add_segment(const Segment& segment);
    void add_node(std::size_t node);
    [[nodiscard]] std::optional<double> cost_if_feasible() const;
    [[nodiscard]] double cost_unchecked() const;

private:
    const Instance& instance_;
    std::size_t taxi_;
    const RouteCache& cache_;
    std::optional<Segment> summary_;
};
} // namespace soict::alns
