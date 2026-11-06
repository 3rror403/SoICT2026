#pragma once

#include "model/Instance.hpp"
#include <limits>

namespace soict::alns {

// Common request view used by route generation and local-search support.
// Request ids list passengers first, then parcels.
struct RequestData {
    std::size_t pickup = 0, delivery = 0;
    double ready = 0, latest = 0;
    double drop_ready = 0, drop_latest = 0;
    double service = 0, revenue = 0, weight = 0;
};

inline RequestData request_data(const Instance& in, std::size_t request) {
    const auto n = in.passengers.size();
    if (request < n) {
        const auto& p = in.passengers[request];
        return {in.passenger_pickup(request), in.passenger_dropoff(request),
                p.pickup.earliest, p.pickup.latest, 0,
                std::numeric_limits<double>::infinity(), p.service, p.revenue, 0};
    }
    const auto& p = in.parcels[request - n];
    return {in.parcel_pickup(request - n), in.parcel_dropoff(request - n),
            p.pickup.earliest, p.pickup.latest, p.delivery.earliest,
            p.delivery.latest, p.service, p.revenue, p.weight};
}

inline std::size_t request_id(const Instance& in, std::size_t node) {
    const auto k = in.taxis.size(), n = in.passengers.size(), m = in.parcels.size();
    return node < k + 2 * n ? (node - k) % n : n + (node - k - 2 * n) % m;
}

} // namespace soict::alns
