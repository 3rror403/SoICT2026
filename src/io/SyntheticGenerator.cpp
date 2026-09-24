#include "io/SyntheticGenerator.hpp"

#include <cmath>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

namespace soict {
Instance generate_synthetic_instance(const GeneratorOptions& options) {
    if (options.taxis == 0 || options.taxis > 1000 ||
        options.passengers + options.parcels == 0 ||
        options.passengers + options.parcels > 5000)
        throw std::invalid_argument("invalid generator size");

    std::mt19937_64 rng(options.seed);
    Instance instance;
    instance.taxis.resize(options.taxis);
    instance.passengers.resize(options.passengers);
    instance.parcels.resize(options.parcels);
    const auto v = instance.node_count();
    instance.travel_time.assign(v * v, 0);
    instance.travel_cost.assign(v * v, 0);

    // Replace the unavailable San Francisco traces and road graph with seeded
    // synthetic points. The directed matrices follow the contest input model.
    std::uniform_real_distribution<double> coordinate(0, 100);
    std::uniform_real_distribution<double> directed_factor(0.9, 1.1);
    std::vector<std::pair<double, double>> points(v);
    for (auto& [x, y] : points) {
        x = coordinate(rng);
        y = coordinate(rng);
    }
    for (std::size_t u = 0; u < v; ++u) {
        for (std::size_t w = 0; w < v; ++w) {
            if (u == w) continue;
            const double distance = std::hypot(points[u].first - points[w].first,
                                               points[u].second - points[w].second);
            instance.travel_time[u * v + w] = distance * directed_factor(rng);
            instance.travel_cost[u * v + w] = distance * directed_factor(rng);
        }
    }

    // The paper uses a roughly half passenger / half parcel mix and pickup
    // windows around observed trip times. Here the counts are explicit and the
    // hypothetical trip times are generated in minutes within a 10-hour day.
    std::uniform_real_distribution<double> pickup_time(10, 480);
    std::uniform_real_distribution<double> service_time(1, 5);
    std::uniform_real_distribution<double> revenue_factor(1.15, 2.5);
    std::uniform_int_distribution<int> parcel_weight(1, 3);
    for (auto& taxi : instance.taxis) taxi.parcel_capacity = 5;
    for (std::size_t i = 0; i < options.passengers; ++i) {
        auto& p = instance.passengers[i];
        const double t = pickup_time(rng);
        p.pickup = {t - 10, t + 15};
        p.service = service_time(rng);
        const double direct_cost = instance.cost(instance.passenger_pickup(i),
                                                 instance.passenger_dropoff(i));
        p.revenue = (1 + direct_cost) * revenue_factor(rng);
    }
    for (std::size_t j = 0; j < options.parcels; ++j) {
        auto& p = instance.parcels[j];
        const double t = pickup_time(rng);
        const double direct_time = instance.time(instance.parcel_pickup(j),
                                                 instance.parcel_dropoff(j));
        p.pickup = {t - 10, t + 60};
        const double call = t - 10;
        p.delivery = {call + direct_time, call + direct_time + 30};
        p.service = service_time(rng);
        p.weight = parcel_weight(rng);
        const double direct_cost = instance.cost(instance.parcel_pickup(j),
                                                 instance.parcel_dropoff(j));
        p.revenue = (1 + direct_cost) * revenue_factor(rng);
    }
    instance.validate();
    return instance;
}
} // namespace soict
