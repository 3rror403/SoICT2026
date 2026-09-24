#include "io/InstanceIO.hpp"

#include <iomanip>
#include <istream>
#include <limits>
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
    std::string magic;
    int version = 0;
    read(in, magic, "magic");
    read(in, version, "version");
    if (magic != "SOICT2026" || version != 1)
        throw std::runtime_error("unsupported instance format");
    std::size_t k = 0, n = 0, m = 0;
    read(in, k, "taxi count");
    read(in, n, "passenger count");
    read(in, m, "parcel count");
    if (k == 0 || k > 100000 || n > 100000 || m > 100000 ||
        n + m == 0 || n + m > 20000)
        throw std::runtime_error("invalid or unsupported instance size");
    const std::size_t v = k + 2 * (n + m);
    if (v > 50000 || v > std::numeric_limits<std::size_t>::max() / v)
        throw std::runtime_error("node count too large");
    Instance instance;
    instance.taxis.resize(k);
    instance.passengers.resize(n);
    instance.parcels.resize(m);
    for (auto& taxi : instance.taxis) read(in, taxi.parcel_capacity, "taxi capacity");
    for (auto& p : instance.passengers) {
        read(in, p.revenue, "passenger revenue");
        read(in, p.pickup.earliest, "passenger earliest");
        read(in, p.pickup.latest, "passenger latest");
        read(in, p.service, "passenger service");
    }
    for (auto& p : instance.parcels) {
        read(in, p.revenue, "parcel revenue");
        read(in, p.weight, "parcel weight");
        read(in, p.pickup.earliest, "parcel pickup earliest");
        read(in, p.pickup.latest, "parcel pickup latest");
        read(in, p.delivery.earliest, "parcel delivery earliest");
        read(in, p.delivery.latest, "parcel delivery latest");
        read(in, p.service, "parcel service");
    }
    instance.travel_time.resize(v * v);
    instance.travel_cost.resize(v * v);
    for (double& x : instance.travel_time) read(in, x, "travel time");
    for (double& x : instance.travel_cost) read(in, x, "travel cost");
    instance.validate();
    return instance;
}

void write_instance(std::ostream& out, const Instance& instance) {
    instance.validate();
    out << std::setprecision(17);
    out << "SOICT2026 1\n" << instance.taxis.size() << ' '
        << instance.passengers.size() << ' ' << instance.parcels.size() << '\n';
    for (const auto& taxi : instance.taxis) out << taxi.parcel_capacity << '\n';
    for (const auto& p : instance.passengers)
        out << p.revenue << ' ' << p.pickup.earliest << ' '
            << p.pickup.latest << ' ' << p.service << '\n';
    for (const auto& p : instance.parcels)
        out << p.revenue << ' ' << p.weight << ' '
            << p.pickup.earliest << ' ' << p.pickup.latest << ' '
            << p.delivery.earliest << ' ' << p.delivery.latest << ' '
            << p.service << '\n';
    const auto v = instance.node_count();
    for (const auto* matrix : {&instance.travel_time, &instance.travel_cost}) {
        for (std::size_t i = 0; i < v; ++i) {
            for (std::size_t j = 0; j < v; ++j) {
                if (j) out << ' ';
                out << (*matrix)[i * v + j];
            }
            out << '\n';
        }
    }
    if (!out) throw std::runtime_error("failed to write instance");
}
} // namespace soict
