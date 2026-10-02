#include "model/Instance.hpp"

#include <cmath>
#include <limits>

namespace soict {
namespace {
void nonnegative(double x, const char* name) {
    if (!std::isfinite(x) || x < 0) throw std::invalid_argument(name);
}
void valid_window(Window w) {
    nonnegative(w.earliest, "negative or nonfinite window start");
    nonnegative(w.latest, "negative or nonfinite window end");
    if (w.earliest > w.latest) throw std::invalid_argument("reversed time window");
}
}

void Instance::validate() const {
    if (taxis.empty()) throw std::invalid_argument("instance needs at least one taxi");
    const auto v = node_count();
    if (!vertex_ids.empty()) {
        if (vertex_ids.size() != v) throw std::invalid_argument("invalid vertex mapping size");
        std::vector<bool> seen(v, false);
        for (const auto id : vertex_ids) {
            if (id == 0 || id > v || seen[id - 1])
                throw std::invalid_argument("vertex IDs must be a permutation of 1..V");
            seen[id - 1] = true;
        }
    }
    if (v > std::numeric_limits<std::size_t>::max() / v ||
        travel_time.size() != v * v || travel_cost.size() != v * v)
        throw std::invalid_argument("matrix dimensions do not match node count");
    for (const auto& taxi : taxis) nonnegative(taxi.parcel_capacity, "invalid taxi capacity");
    for (const auto& p : passengers) {
        nonnegative(p.revenue, "invalid passenger revenue");
        nonnegative(p.service, "invalid passenger service duration");
        valid_window(p.pickup);
    }
    for (const auto& p : parcels) {
        nonnegative(p.revenue, "invalid parcel revenue");
        if (!std::isfinite(p.weight) || p.weight <= 0)
            throw std::invalid_argument("invalid parcel weight");
        nonnegative(p.service, "invalid parcel service duration");
        valid_window(p.pickup);
        valid_window(p.delivery);
    }
    for (double x : travel_time) nonnegative(x, "invalid travel time");
    for (double x : travel_cost) nonnegative(x, "invalid travel cost");
}
} // namespace soict
