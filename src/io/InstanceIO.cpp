#include "io/InstanceIO.hpp"

#include <iomanip>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <string>

namespace soict {
namespace {
template <typename T> void read(std::istream& in, T& value, const char* field) {
    if (!(in >> value)) throw std::runtime_error(std::string("cannot read ") + field);
}
}

Instance read_instance(std::istream& in) {
    std::size_t k = 0, n = 0, m = 0;
    read(in, k, "taxi count");
    read(in, n, "passenger count");
    read(in, m, "parcel count");
    if (k == 0 || k > 100000 || n > 20000 || m > 20000 || n + m > 20000)
        throw std::runtime_error("invalid or unsupported instance size");
    const std::size_t v = k + 2 * (n + m);
    if (v > 50000) throw std::runtime_error("node count too large");
    Instance instance;
    instance.taxis.resize(k);
    instance.passengers.resize(n);
    instance.parcels.resize(m);
    instance.vertex_ids.resize(v);
    std::vector<bool> seen(v, false);
    const auto vertex = [&](std::size_t node) {
        auto& id = instance.vertex_ids[node];
        read(in, id, "vertex ID");
        if (id == 0 || id > v || seen[id - 1])
            throw std::runtime_error("vertex IDs must be distinct and within 1..V");
        seen[id - 1] = true;
    };
    for (std::size_t taxi = 0; taxi < k; ++taxi) {
        vertex(taxi);
        read(in, instance.taxis[taxi].parcel_capacity, "taxi capacity");
    }
    for (std::size_t i = 0; i < n; ++i) {
        auto& p = instance.passengers[i];
        vertex(instance.passenger_pickup(i));
        vertex(instance.passenger_dropoff(i));
        read(in, p.pickup.earliest, "passenger earliest");
        read(in, p.pickup.latest, "passenger latest");
        read(in, p.service, "passenger service");
        read(in, p.revenue, "passenger revenue");
    }
    for (std::size_t j = 0; j < m; ++j) {
        auto& p = instance.parcels[j];
        vertex(instance.parcel_pickup(j));
        vertex(instance.parcel_dropoff(j));
        read(in, p.weight, "parcel weight");
        read(in, p.pickup.earliest, "parcel pickup earliest");
        read(in, p.pickup.latest, "parcel pickup latest");
        read(in, p.delivery.earliest, "parcel delivery earliest");
        read(in, p.delivery.latest, "parcel delivery latest");
        read(in, p.service, "parcel service");
        read(in, p.revenue, "parcel revenue");
    }
    std::vector<std::size_t> internal(v);
    for (std::size_t node = 0; node < v; ++node)
        internal[instance.vertex_id(node) - 1] = node;
    for (auto* matrix : {&instance.travel_time, &instance.travel_cost}) {
        matrix->resize(v * v);
        for (std::size_t u = 0; u < v; ++u)
            for (std::size_t w = 0; w < v; ++w)
                read(in, (*matrix)[internal[u] * v + internal[w]], "matrix entry");
    }
    std::string extra;
    if (in >> extra) throw std::runtime_error("unexpected data after instance matrices");
    instance.validate();
    return instance;
}

void write_instance(std::ostream& out, const Instance& instance) {
    instance.validate();
    out << std::setprecision(17);
    out << instance.taxis.size() << ' ' << instance.passengers.size() << ' '
        << instance.parcels.size() << '\n';
    for (std::size_t k = 0; k < instance.taxis.size(); ++k)
        out << instance.vertex_id(k) << ' ' << instance.taxis[k].parcel_capacity << '\n';
    for (std::size_t i = 0; i < instance.passengers.size(); ++i) {
        const auto& p = instance.passengers[i];
        out << instance.vertex_id(instance.passenger_pickup(i)) << ' '
            << instance.vertex_id(instance.passenger_dropoff(i)) << ' '
            << p.pickup.earliest << ' ' << p.pickup.latest << ' '
            << p.service << ' ' << p.revenue << '\n';
    }
    for (std::size_t j = 0; j < instance.parcels.size(); ++j) {
        const auto& p = instance.parcels[j];
        out << instance.vertex_id(instance.parcel_pickup(j)) << ' '
            << instance.vertex_id(instance.parcel_dropoff(j)) << ' ' << p.weight << ' '
            << p.pickup.earliest << ' ' << p.pickup.latest << ' '
            << p.delivery.earliest << ' ' << p.delivery.latest << ' '
            << p.service << ' ' << p.revenue << '\n';
    }
    const auto v = instance.node_count();
    std::vector<std::size_t> internal(v);
    for (std::size_t node = 0; node < v; ++node)
        internal[instance.vertex_id(node) - 1] = node;
    for (const auto* matrix : {&instance.travel_time, &instance.travel_cost}) {
        for (std::size_t u = 0; u < v; ++u) {
            for (std::size_t w = 0; w < v; ++w) {
                if (w) out << ' ';
                out << (*matrix)[internal[u] * v + internal[w]];
            }
            out << '\n';
        }
    }
    if (!out) throw std::runtime_error("failed to write instance");
}
} // namespace soict
