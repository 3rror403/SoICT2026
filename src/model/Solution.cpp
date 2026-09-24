#include "model/Solution.hpp"

#include <algorithm>
#include <cmath>

namespace soict {
Evaluation Solution::evaluate(const Instance& instance) const {
    if (routes.size() != instance.taxis.size())
        return {false, 0, "route count differs from taxi count"};
    const auto n = instance.passengers.size();
    const auto m = instance.parcels.size();
    const auto k_count = instance.taxis.size();
    const auto v_count = instance.node_count();
    std::vector<int> pickup_owner(n + m, -1);
    std::vector<int> drop_owner(n + m, -1);
    double benefit = 0;
    constexpr double eps = 1e-8;

    for (std::size_t k = 0; k < k_count; ++k) {
        std::size_t previous = k;
        double departure = 0;
        double load = 0;
        const auto& route = routes[k];
        for (std::size_t pos = 0; pos < route.size(); ++pos) {
            const auto node = route[pos];
            if (node < k_count || node >= v_count)
                return {false, 0, "route contains invalid service node"};
            const double arrival = departure + instance.time(previous, node);
            if (!std::isfinite(arrival))
                return {false, 0, "nonfinite route time"};
            benefit -= instance.cost(previous, node);

            if (node < k_count + n) {
                const auto i = node - k_count;
                if (pickup_owner[i] != -1)
                    return {false, 0, "passenger picked up more than once"};
                const auto& p = instance.passengers[i];
                if (arrival > p.pickup.latest + eps)
                    return {false, 0, "passenger pickup is late"};
                if (pos + 1 == route.size() ||
                    route[pos + 1] != instance.passenger_dropoff(i))
                    return {false, 0, "passenger drop-off must immediately follow pickup"};
                pickup_owner[i] = static_cast<int>(k);
                departure = std::max(arrival, p.pickup.earliest) + p.service;
            } else if (node < k_count + 2 * n) {
                const auto i = node - k_count - n;
                if (pickup_owner[i] != static_cast<int>(k) || drop_owner[i] != -1)
                    return {false, 0, "invalid passenger drop-off"};
                drop_owner[i] = static_cast<int>(k);
                departure = arrival + instance.passengers[i].service;
            } else if (node < k_count + 2 * n + m) {
                const auto j = node - k_count - 2 * n;
                const auto id = n + j;
                if (pickup_owner[id] != -1)
                    return {false, 0, "parcel picked up more than once"};
                const auto& p = instance.parcels[j];
                if (arrival > p.pickup.latest + eps)
                    return {false, 0, "parcel pickup is late"};
                load += p.weight;
                if (load > instance.taxis[k].parcel_capacity + eps)
                    return {false, 0, "parcel capacity exceeded"};
                pickup_owner[id] = static_cast<int>(k);
                departure = std::max(arrival, p.pickup.earliest) + p.service;
            } else {
                const auto j = node - k_count - 2 * n - m;
                const auto id = n + j;
                if (pickup_owner[id] != static_cast<int>(k) || drop_owner[id] != -1)
                    return {false, 0, "invalid parcel drop-off"};
                const auto& p = instance.parcels[j];
                if (arrival > p.delivery.latest + eps)
                    return {false, 0, "parcel delivery is late"};
                load -= p.weight;
                drop_owner[id] = static_cast<int>(k);
                departure = std::max(arrival, p.delivery.earliest) + p.service;
            }
            previous = node;
        }
    }
    for (std::size_t i = 0; i < n + m; ++i) {
        if (pickup_owner[i] != drop_owner[i])
            return {false, 0, "partially served request"};
        if (pickup_owner[i] != -1)
            benefit += i < n ? instance.passengers[i].revenue :
                                instance.parcels[i - n].revenue;
    }
    return {true, benefit, {}};
}
} // namespace soict
