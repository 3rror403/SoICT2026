#pragma once
#include "model/Instance.hpp"
#include <cstddef>
#include <vector>

namespace soict::alns {
// A route viewed before or after removing one request's two endpoints.
// Reading a virtual position takes O(1), with no copied node vector.
struct RouteView {
    const std::vector<std::size_t>& nodes;
    std::size_t pickup_removed = 0, delivery_removed = 0;
    bool removed = false;

    [[nodiscard]] std::size_t size() const { return nodes.size() - (removed ? 2 : 0); }
    [[nodiscard]] std::size_t node(std::size_t position) const {
        if (!removed) return nodes[position];
        if (position < pickup_removed) return nodes[position];
        if (position < delivery_removed - 1) return nodes[position + 1];
        return nodes[position + 2];
    }
};

// Exact travel-cost saving from deleting two ordered endpoints. The adjacent
// case replaces one chain; the separated case replaces two independent arcs.
inline double removal_savings_cost(const Instance& in, std::size_t taxi,
    const std::vector<std::size_t>& nodes, std::size_t pickup_at,
    std::size_t delivery_at) {
    const auto pickup = nodes[pickup_at], delivery = nodes[delivery_at];
    const auto before_pickup = pickup_at ? nodes[pickup_at - 1] : taxi;
    if (delivery_at == pickup_at + 1) {
        double saving = in.cost(before_pickup, pickup) + in.cost(pickup, delivery);
        if (delivery_at + 1 < nodes.size()) {
            const auto after = nodes[delivery_at + 1];
            saving += in.cost(delivery, after) - in.cost(before_pickup, after);
        }
        return saving;
    }
    const auto after_pickup = nodes[pickup_at + 1];
    const auto before_delivery = nodes[delivery_at - 1];
    double saving = in.cost(before_pickup, pickup) + in.cost(pickup, after_pickup)
                  - in.cost(before_pickup, after_pickup)
                  + in.cost(before_delivery, delivery);
    if (delivery_at + 1 < nodes.size()) {
        const auto after_delivery = nodes[delivery_at + 1];
        saving += in.cost(delivery, after_delivery)
                - in.cost(before_delivery, after_delivery);
    }
    return saving;
}

// Exact directed travel-cost increase caused by replacing one or two arcs.
// A position at route end has no outgoing arc because routes do not return.
inline double single_insertion_cost(const Instance& in, std::size_t taxi,
    const RouteView& route, std::size_t at, std::size_t node) {
    const auto previous = at ? route.node(at - 1) : taxi;
    double delta = in.cost(previous, node);
    if (at < route.size()) {
        const auto next = route.node(at);
        delta += in.cost(node, next) - in.cost(previous, next);
    }
    return delta;
}

inline double adjacent_insertion_cost(const Instance& in, std::size_t taxi,
    const RouteView& route, std::size_t at, std::size_t pickup, std::size_t delivery) {
    const auto previous = at ? route.node(at - 1) : taxi;
    double delta = in.cost(previous, pickup) + in.cost(pickup, delivery);
    if (at < route.size()) {
        const auto next = route.node(at);
        delta += in.cost(delivery, next) - in.cost(previous, next);
    }
    return delta;
}

inline double separated_insertion_cost(const Instance& in, std::size_t taxi,
    const RouteView& route, std::size_t pickup_at, std::size_t delivery_at,
    std::size_t pickup, std::size_t delivery) {
    if (pickup_at == delivery_at)
        return adjacent_insertion_cost(in, taxi, route, pickup_at, pickup, delivery);
    return single_insertion_cost(in, taxi, route, pickup_at, pickup) +
           single_insertion_cost(in, taxi, route, delivery_at, delivery);
}
} // namespace soict::alns
