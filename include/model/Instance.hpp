#pragma once

#include <cstddef>
#include <stdexcept>
#include <vector>

namespace soict {

struct Window {
    double earliest = 0;
    double latest = 0;
};

struct Taxi {
    double parcel_capacity = 0;
};

struct Passenger {
    double revenue = 0;
    Window pickup;
    double service = 0;
};

struct Parcel {
    double revenue = 0;
    double weight = 0;
    Window pickup;
    Window delivery;
    double service = 0;
};

// Node order: taxi origins, passenger pickups, passenger drop-offs,
// parcel pickups, parcel drop-offs. All indices are zero-based.
struct Instance {
    std::vector<Taxi> taxis;
    std::vector<Passenger> passengers;
    std::vector<Parcel> parcels;
    std::vector<double> travel_time;
    std::vector<double> travel_cost;

    [[nodiscard]] std::size_t node_count() const {
        return taxis.size() + 2 * (passengers.size() + parcels.size());
    }
    [[nodiscard]] std::size_t passenger_pickup(std::size_t i) const {
        return taxis.size() + i;
    }
    [[nodiscard]] std::size_t passenger_dropoff(std::size_t i) const {
        return taxis.size() + passengers.size() + i;
    }
    [[nodiscard]] std::size_t parcel_pickup(std::size_t j) const {
        return taxis.size() + 2 * passengers.size() + j;
    }
    [[nodiscard]] std::size_t parcel_dropoff(std::size_t j) const {
        return taxis.size() + 2 * passengers.size() + parcels.size() + j;
    }
    [[nodiscard]] double time(std::size_t u, std::size_t v) const {
        return travel_time.at(u * node_count() + v);
    }
    [[nodiscard]] double cost(std::size_t u, std::size_t v) const {
        return travel_cost.at(u * node_count() + v);
    }
    void validate() const;
};

} // namespace soict
