#include "io/SyntheticGenerator.hpp"
#include "SyntheticCalibration.hpp"

#include <algorithm>
#include <cmath>
#include <random>
#include <stdexcept>
#include <vector>

namespace soict {
Instance generate_synthetic_instance(const GeneratorOptions& options) {
    if (options.taxis == 0 || options.taxis > 1000 ||
        options.passengers > 5000 || options.parcels > 5000 ||
        options.passengers + options.parcels == 0 ||
        options.passengers + options.parcels > 5000)
        throw std::invalid_argument("invalid generator size");
    std::mt19937_64 rng(options.seed);
    std::uniform_real_distribution<double> coordinate(0, 100), noise(0.9, 1.1);
    std::uniform_int_distribution<int> jitter(-2, 2);
    const auto select = [&](const auto& pool) -> const auto& {
        return pool[std::uniform_int_distribution<std::size_t>(0, pool.size()-1)(rng)];
    };
    const auto draw_case = [&]() -> const auto& { return select(calibration::cases); };
    const auto width = [&](double base) { return std::max(1.0, std::round(base*noise(rng))); };
    const auto revenue = [](double c, double slope, double intercept, double residual) {
        return std::max(1.0, std::round(slope*c + intercept + residual));
    };
    Instance instance;
    instance.taxis.resize(options.taxis);
    instance.passengers.resize(options.passengers);
    instance.parcels.resize(options.parcels);
    const auto v = instance.node_count();
    instance.vertex_ids.resize(v);
    instance.travel_time.assign(v*v, 0);
    instance.travel_cost.assign(v*v, 0);
    struct Point { double x, y; };
    std::vector<Point> points(v);
    const auto draw_point = [&]() { return Point{coordinate(rng), coordinate(rng)}; };
    for (auto& p : points) p = draw_point();
    const auto distance = [&](std::size_t u, std::size_t w) {
        return std::hypot(points[u].x-points[w].x, points[u].y-points[w].y);
    };
    for (std::size_t k = 0; k < options.taxis; ++k) {
        instance.vertex_ids[k] = k+1;
        instance.taxis[k].parcel_capacity = select(draw_case().capacities);
    }
    std::vector<double> passenger_residual(options.passengers), parcel_residual(options.parcels), delta(options.parcels);
    for (std::size_t i = 0; i < options.passengers; ++i) {
        const auto p = instance.passenger_pickup(i), d = instance.passenger_dropoff(i);
        instance.vertex_ids[p] = options.taxis+2*i+1;
        instance.vertex_ids[d] = options.taxis+2*i+2;
        const auto& record = select(draw_case().passengers);
        auto& request = instance.passengers[i];
        const double e = std::clamp(record.earliest+jitter(rng), 0.0, 100.0);
        request.pickup = {e, e+width(record.width)};
        request.service = record.service;
        passenger_residual[i] = record.residual;
    }
    for (std::size_t j = 0; j < options.parcels; ++j) {
        const auto p = instance.parcel_pickup(j), d = instance.parcel_dropoff(j);
        instance.vertex_ids[p] = options.taxis+2*options.passengers+2*j+1;
        instance.vertex_ids[d] = options.taxis+2*options.passengers+2*j+2;
        const auto& record = select(draw_case().parcels);
        auto& request = instance.parcels[j];
        const double e = std::clamp(record.earliest+jitter(rng), 0.0, 90.0);
        request.pickup = {e, e+width(record.width)};
        request.delivery = {0, width(record.delivery_width)};
        request.service = record.service;
        request.weight = record.weight;
        parcel_residual[j] = record.residual;
        delta[j] = record.delta;
    }
    // Condition pickup locations on reachability without changing sampled windows.
    const auto reachable_pickup = [&](std::size_t p, double latest) {
        const double margin = std::min(5.0, latest-1.0);
        const double factor = options.directed_noise ? 1.1 : 1.0;
        const auto reachable = [&]() {
            for (std::size_t k = 0; k < options.taxis; ++k)
                if (std::ceil(distance(k,p))*factor <= latest-margin) return true;
            return false;
        };
        for (int attempts = 0; attempts < 4096 && !reachable(); ++attempts)
            points[p] = draw_point();
        if (!reachable()) points[p] = points[std::uniform_int_distribution<std::size_t>(0, options.taxis-1)(rng)];
    };
    for (std::size_t i = 0; i < options.passengers; ++i)
        reachable_pickup(instance.passenger_pickup(i), instance.passengers[i].pickup.latest);
    for (std::size_t j = 0; j < options.parcels; ++j)
        reachable_pickup(instance.parcel_pickup(j),
                         std::min(instance.parcels[j].pickup.latest,
                                  instance.parcels[j].pickup.earliest+delta[j]+instance.parcels[j].delivery.latest));
    for (std::size_t u = 0; u < v; ++u) {
        for (std::size_t w = u+1; w < v; ++w) {
            const double d = distance(u,w);
            const double t = std::max(1.0, std::ceil(d));
            const double c = std::max(1.0, std::ceil(2.48*d));
            for (const auto index : {u*v+w, w*v+u}) {
                const double factor = options.directed_noise ? noise(rng) : 1.0;
                instance.travel_time[index] = std::max(1.0, std::round(t*factor));
                instance.travel_cost[index] = std::max(1.0, std::round(c*factor));
            }
        }
    }
    for (std::size_t i = 0; i < options.passengers; ++i) {
        auto& r = instance.passengers[i];
        r.revenue = revenue(instance.cost(instance.passenger_pickup(i),instance.passenger_dropoff(i)),
                            calibration::passenger_slope, calibration::passenger_intercept, passenger_residual[i]);
    }
    for (std::size_t j = 0; j < options.parcels; ++j) {
        auto& r = instance.parcels[j];
        const auto p = instance.parcel_pickup(j), d = instance.parcel_dropoff(j);
        r.delivery.earliest = r.pickup.earliest+r.service+instance.time(p,d)+delta[j];
        r.delivery.latest += r.delivery.earliest;
        r.revenue = revenue(instance.cost(p,d),calibration::parcel_slope,calibration::parcel_intercept,parcel_residual[j]);
    }
    instance.validate();
    return instance;
}
} // namespace soict
