// Generated judge package; C++17; no external libraries.
#include <cstddef>
#include <limits>
#include <algorithm>
#include <vector>
template<class T, class P> void soict_erase_if(std::vector<T>& values, P predicate) {
values.erase(std::remove_if(values.begin(), values.end(), predicate), values.end());
}
inline std::size_t soict_bit_width(std::size_t value) {
return value ? std::numeric_limits<unsigned long long>::digits - __builtin_clzll(static_cast<unsigned long long>(value)) : 0;
}

// BEGIN src/model/Instance.cpp

// BEGIN include/model/Instance.hpp

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
    // Internal node -> official one-based vertex. Empty means node + 1.
    std::vector<std::size_t> vertex_ids;
    std::vector<double> travel_time;
    std::vector<double> travel_cost;

    [[nodiscard]] std::size_t node_count() const {
        return taxis.size() + 2 * (passengers.size() + parcels.size());
    }
    [[nodiscard]] std::size_t vertex_id(std::size_t node) const {
        return vertex_ids.empty() ? node + 1 : vertex_ids.at(node);
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

// END include/model/Instance.hpp

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

// END src/model/Instance.cpp

// BEGIN src/model/Solution.cpp

// BEGIN include/model/Solution.hpp

#include <string>
#include <vector>

namespace soict {

struct Evaluation {
    bool feasible = false;
    double benefit = 0;
    std::string error;
};

// routes[k] lists service-node indices after taxi k's origin.
struct Solution {
    std::vector<std::vector<std::size_t>> routes;

    [[nodiscard]] Evaluation evaluate(const Instance& instance) const;
};

} // namespace soict

// END include/model/Solution.hpp

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

// END src/model/Solution.cpp

// BEGIN src/io/InstanceIO.cpp

// BEGIN include/io/InstanceIO.hpp

#include <iosfwd>

namespace soict {
Instance read_instance(std::istream& in);
void write_instance(std::ostream& out, const Instance& instance);
}

// END include/io/InstanceIO.hpp

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

} // namespace soict

// END src/io/InstanceIO.cpp

// BEGIN src/io/SolutionIO.cpp

// BEGIN include/io/SolutionIO.hpp

#include <iosfwd>

namespace soict {

Solution read_solution(std::istream& in, const Instance& instance);
void write_solution(std::ostream& out, const Solution& solution, const Instance& instance);

} // namespace soict

// END include/io/SolutionIO.hpp

#include <cmath>
#include <iomanip>
#include <istream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace soict {
void write_solution(std::ostream& out, const Solution& solution, const Instance& instance) {
    instance.validate();
    const auto evaluation = solution.evaluate(instance);
    if (!evaluation.feasible) throw std::runtime_error("invalid solution: " + evaluation.error);
    out << std::setprecision(17) << evaluation.benefit << '\n';
    for (const auto& route : solution.routes) {
        out << route.size();
        for (const auto node : route) out << ' ' << instance.vertex_id(node);
        out << '\n';
    }
    if (!out) throw std::runtime_error("failed to write solution");
}
} // namespace soict

// END src/io/SolutionIO.cpp

// BEGIN src/search/alns/RouteCache.cpp

// BEGIN include/search/alns/RouteCache.hpp

#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <vector>

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

// END include/search/alns/RouteCache.hpp

#include <algorithm>
#include <atomic>
#include <cmath>
#include <stdexcept>

namespace soict::alns {
namespace {
std::atomic<std::uint64_t> next_route_version{1};
double service(const Instance& in, std::size_t node) {
    const auto k = in.taxis.size(), n = in.passengers.size(), m = in.parcels.size();
    if (node < k || node >= in.node_count()) throw std::logic_error("invalid service node");
    if (node < k + 2 * n) return in.passengers[(node - k) % n].service;
    return in.parcels[(node - k - 2 * n) % m].service;
}
double load_delta(const Instance& in, std::size_t node) {
    const auto k = in.taxis.size(), n = in.passengers.size(), m = in.parcels.size();
    if (node < k + 2 * n) return 0;
    const auto j = (node - k - 2 * n) % m;
    return node < k + 2 * n + m ? in.parcels[j].weight : -in.parcels[j].weight;
}
} // namespace

Segment single_node(const Instance& in, std::size_t node) {
    Segment s;
    s.first = s.last = node;
    s.duration = service(in, node);
    const auto k = in.taxis.size(), n = in.passengers.size(), m = in.parcels.size();
    const Window* window = nullptr;
    if (node < k + n) window = &in.passengers[node - k].pickup;
    else if (node >= k + 2 * n && node < k + 2 * n + m)
        window = &in.parcels[node - k - 2 * n].pickup;
    else if (node >= k + 2 * n + m)
        window = &in.parcels[node - k - 2 * n - m].delivery;
    if (window) {
        s.ready = window->earliest + s.duration;
        s.latest_input = window->latest;
    }
    s.load_change = load_delta(in, node);
    s.min_load = std::min(0.0, s.load_change);
    s.max_load = std::max(0.0, s.load_change);
    return s;
}

Segment join(const Instance& in, const Segment& a, const Segment& b) {
    const double travel = in.time(a.last, b.first);
    Segment s;
    s.first = a.first;
    s.last = b.last;
    s.duration = a.duration + travel + b.duration;
    s.ready = std::max(a.ready + travel + b.duration, b.ready);
    s.latest_input = std::min(a.latest_input, b.latest_input - travel - a.duration);
    s.feasible = a.feasible && b.feasible && a.ready + travel <= b.latest_input;
    s.cost = a.cost + in.cost(a.last, b.first) + b.cost;
    s.load_change = a.load_change + b.load_change;
    s.min_load = std::min(a.min_load, a.load_change + b.min_load);
    s.max_load = std::max(a.max_load, a.load_change + b.max_load);
    return s;
}

RouteCache::RouteCache(const Instance& in, std::size_t taxi,
                       const std::vector<std::size_t>& route) {
    rebuild(in, taxi, route);
}

void RouteCache::clear() {
    nodes_.clear(); singles_.clear();
    arrival_.clear(); departure_.clear(); load_.clear();
    for (auto& level : table_) level.clear();
    cost_ = 0;
    version_ = 0;
}

void RouteCache::copy_from(const RouteCache& other) {
    if (this == &other) return;
    nodes_ = other.nodes_;
    singles_ = other.singles_;
    arrival_ = other.arrival_; departure_ = other.departure_; load_ = other.load_;
    if (table_.size() < other.table_.size()) table_.resize(other.table_.size());
    for (std::size_t i = 0; i < other.table_.size(); ++i) table_[i] = other.table_[i];
    for (std::size_t i = other.table_.size(); i < table_.size(); ++i) table_[i].clear();
    cost_ = other.cost_;
    version_ = other.version_;
}

void RouteCache::rebuild(const Instance& in, std::size_t taxi,
                          const std::vector<std::size_t>& route) {
    if (&route != &nodes_) nodes_ = route;
    singles_.clear(); arrival_.clear(); departure_.clear(); load_.clear();
    for (auto& level : table_) level.clear();
    cost_ = 0;
    version_ = next_route_version.fetch_add(1, std::memory_order_relaxed);
    const auto size = route.size();
    singles_.reserve(size);
    arrival_.reserve(size); departure_.reserve(size); load_.reserve(size);
    std::size_t previous = taxi;
    double departure = 0, load = 0;
    for (const auto node : route) {
        singles_.push_back(single_node(in, node));
        const double arrival = departure + in.time(previous, node);
        departure = std::max(arrival + singles_.back().duration, singles_.back().ready);
        load += singles_.back().load_change;
        cost_ += in.cost(previous, node);
        arrival_.push_back(arrival);
        departure_.push_back(departure);
        load_.push_back(load);
        previous = node;
    }
    // Disjoint sparse table: associative, order-preserving range summary in O(1).
    std::size_t level_index = 0;
    for (std::size_t half = 1; half < size; half *= 2, ++level_index) {
        if (level_index == table_.size()) table_.emplace_back();
        auto& level = table_[level_index];
        level.resize(size);
        const std::size_t block = half * 2;
        for (std::size_t start = 0; start < size; start += block) {
            const auto mid = std::min(start + half, size);
            const auto end = std::min(start + block, size);
            if (mid > start) {
                level[mid - 1] = singles_[mid - 1];
                for (auto i = mid - 1; i > start; --i)
                    level[i - 1] = join(in, singles_[i - 1], level[i]);
            }
            if (mid < end) {
                level[mid] = singles_[mid];
                for (auto i = mid + 1; i < end; ++i)
                    level[i] = join(in, level[i - 1], singles_[i]);
            }
        }
        if (half > size / 2) break;
    }
}

Segment RouteCache::range(const Instance& in, std::size_t first, std::size_t last) const {
    if (first > last || last >= nodes_.size()) throw std::out_of_range("invalid route range");
    if (first == last) return singles_[first];
    const auto level = soict_bit_width(first ^ last) - 1;
    return join(in, table_.at(level)[first], table_.at(level)[last]);
}

bool RouteCache::boundary_allowed(const Instance& in, std::size_t pos) const {
    if (pos > nodes_.size()) return false;
    if (pos == 0) return true;
    const auto preceding = nodes_[pos - 1], k = in.taxis.size();
    return preceding < k || preceding >= k + in.passengers.size();
}

RouteCandidate::RouteCandidate(const Instance& in, std::size_t taxi,
                               const RouteCache& cache)
    : instance_(in), taxi_(taxi), cache_(cache) {}

void RouteCandidate::add_range(std::size_t first, std::size_t end) {
    if (first == end) return;
    add_segment(cache_.range(instance_, first, end - 1));
}
void RouteCandidate::add_segment(const Segment& segment) {
    summary_ = summary_ ? join(instance_, *summary_, segment) : segment;
}
void RouteCandidate::add_node(std::size_t node) { add_segment(single_node(instance_, node)); }
std::optional<double> RouteCandidate::cost_if_feasible() const {
    if (!summary_) return 0.0;
    const auto& s = *summary_;
    const double arrival = instance_.time(taxi_, s.first);
    if (!s.feasible || arrival > s.latest_input || s.min_load < -1e-8 ||
        s.max_load > instance_.taxis[taxi_].parcel_capacity + 1e-8)
        return std::nullopt;
    return instance_.cost(taxi_, s.first) + s.cost;
}
double RouteCandidate::cost_unchecked() const {
    if (!summary_) return 0.0;
    return instance_.cost(taxi_, summary_->first) + summary_->cost;
}
} // namespace soict::alns

// END src/search/alns/RouteCache.cpp

// BEGIN src/search/alns/SearchState.cpp

// BEGIN include/search/alns/SearchState.hpp

#include <memory>
#include <optional>
#include <cstdint>
#include <vector>

namespace soict::alns {
class EmptyTaxiIndex;

// Request ids 0..N-1 are passengers; N..N+M-1 are parcels.
struct RequestLocation {
    int taxi = -1;
    std::size_t pickup = 0, dropoff = 0;
};

class SearchState {
public:
    SearchState(const Instance& instance, Solution solution);
    void copy_from(const SearchState& other);
    void swap(SearchState& other) noexcept;
    [[nodiscard]] Solution solution() const;
    [[nodiscard]] const RouteCache& cache(std::size_t taxi) const;
    [[nodiscard]] const std::vector<std::size_t>& active_taxis() const {
        return active_taxis_;
    }
    [[nodiscard]] bool is_active(std::size_t taxi) const {
        return taxi < routes_by_taxi_.size() && !routes_by_taxi_[taxi].nodes().empty();
    }
    [[nodiscard]] std::size_t taxi_count() const { return instance_->taxis.size(); }
    [[nodiscard]] const RequestLocation& location(std::size_t request) const {
        return locations_.at(request);
    }
    [[nodiscard]] std::size_t request_count() const { return locations_.size(); }
    [[nodiscard]] std::size_t passenger_count() const { return instance_->passengers.size(); }
    [[nodiscard]] std::size_t served_count() const;
    [[nodiscard]] double benefit() const { return benefit_; }
    [[nodiscard]] double revenue(std::size_t request) const;
    [[nodiscard]] const Instance& instance() const { return *instance_; }
    [[nodiscard]] std::size_t pickup_node(std::size_t request) const;
    [[nodiscard]] const std::shared_ptr<const EmptyTaxiIndex>& empty_taxi_index() const {
        return empty_taxi_index_;
    }
    void set_empty_taxi_index(std::shared_ptr<const EmptyTaxiIndex> index) {
        empty_taxi_index_ = std::move(index);
    }
    [[nodiscard]] std::optional<double> removal_gain(std::size_t request) const;
    [[nodiscard]] std::optional<double> insertion_gain(std::size_t request,
        std::size_t taxi, std::size_t pickup_boundary,
        std::size_t delivery_boundary) const;

    bool remove(std::size_t request);
    bool insert(std::size_t request, std::size_t taxi,
                std::size_t pickup_boundary, std::size_t delivery_boundary);
    bool relocate_within_route(std::size_t request, std::size_t pickup_boundary,
                               std::size_t delivery_boundary);
    void check() const;

private:
    const Instance* instance_;
    // Empty routes retain their buffers for subsequent activation.
    std::vector<RouteCache> routes_by_taxi_;
    // Ordered iteration preserves move tie order and repair-cache sorting.
    std::vector<std::size_t> active_taxis_;
    std::vector<RequestLocation> locations_;
    struct RemovalEntry {
        std::uint64_t route_version = 0;
        std::optional<double> gain;
    };
    mutable std::vector<RemovalEntry> removal_cache_;
    std::shared_ptr<const EmptyTaxiIndex> empty_taxi_index_;
    double benefit_ = 0;
    std::vector<std::size_t> route_scratch_;
    RouteCache relocation_check_;
    void rebuild_route(std::size_t taxi, const std::vector<std::size_t>& route);
    [[nodiscard]] std::size_t dropoff_node(std::size_t request) const;
};
} // namespace soict::alns

// END include/search/alns/SearchState.hpp

// BEGIN include/search/alns/FastRouteCheck.hpp

#include <algorithm>
#include <optional>

namespace soict::alns {
// Fixed-size feasibility accumulator for a move's unchanged route pieces.
// A piece maps arrival x to departure max(x + duration, ready). Joining pieces
// keeps that map, the latest admissible input, and the parcel-load extrema.
// Travel cost is deliberately absent: each move computes it from changed arcs.
class FastRouteCheck {
public:
    FastRouteCheck(const Instance& in, std::size_t taxi, const RouteCache& route)
        : in_(in), taxi_(taxi), route_(route) {}

    void range(std::size_t first, std::size_t end) {
        if (first < end) append(route_.range(in_, first, end - 1));
    }
    void node(std::size_t id) { append(single_node(in_, id)); }
    void segment(const Segment& next) { append(next); }

    [[nodiscard]] bool feasible() const {
        if (!summary_) return true;
        const auto& s = *summary_;
        return s.feasible && in_.time(taxi_, s.first) <= s.latest_input &&
               s.min_load >= -1e-8 &&
               s.max_load <= in_.taxis[taxi_].parcel_capacity + 1e-8;
    }

private:
    void append(const Segment& next) {
        if (!summary_) {
            summary_ = next;
            return;
        }
        auto& s = *summary_;
        const double travel = in_.time(s.last, next.first);
        const double old_change = s.load_change;
        const double old_duration = s.duration;
        const double old_ready = s.ready;
        s.feasible = s.feasible && next.feasible &&
                     old_ready + travel <= next.latest_input;
        s.latest_input = std::min(s.latest_input,
                                  next.latest_input - travel - old_duration);
        s.duration = old_duration + travel + next.duration;
        s.ready = std::max(old_ready + travel + next.duration, next.ready);
        s.min_load = std::min(s.min_load, old_change + next.min_load);
        s.max_load = std::max(s.max_load, old_change + next.max_load);
        s.load_change = old_change + next.load_change;
        s.last = next.last;
    }

    const Instance& in_;
    std::size_t taxi_;
    const RouteCache& route_;
    std::optional<Segment> summary_;
};
} // namespace soict::alns

// END include/search/alns/FastRouteCheck.hpp

// BEGIN include/search/alns/MoveCost.hpp
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

// END include/search/alns/MoveCost.hpp

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

namespace soict::alns {
SearchState::SearchState(const Instance& in, Solution solution)
    : instance_(&in), routes_by_taxi_(in.taxis.size()),
      locations_(in.passengers.size() + in.parcels.size()),
      removal_cache_(locations_.size()) {
    active_taxis_.reserve(in.taxis.size());
    const auto evaluation = solution.evaluate(in);
    if (!evaluation.feasible) throw std::logic_error("invalid search state: " + evaluation.error);
    benefit_ = evaluation.benefit;
    for (std::size_t k = 0; k < solution.routes.size(); ++k)
        if (!solution.routes[k].empty()) rebuild_route(k, solution.routes[k]);
}

void SearchState::copy_from(const SearchState& other) {
    if (this == &other) return;
    if (instance_ != other.instance_) throw std::logic_error("cannot copy states of different instances");
    for (std::size_t taxi = 0; taxi < routes_by_taxi_.size(); ++taxi)
        routes_by_taxi_[taxi].copy_from(other.routes_by_taxi_[taxi]);
    active_taxis_.reserve(taxi_count());
    active_taxis_ = other.active_taxis_;
    locations_ = other.locations_;
    removal_cache_ = other.removal_cache_;
    empty_taxi_index_ = other.empty_taxi_index_;
    benefit_ = other.benefit_;
}

void SearchState::swap(SearchState& other) noexcept {
    using std::swap;
    swap(instance_, other.instance_);
    routes_by_taxi_.swap(other.routes_by_taxi_);
    active_taxis_.swap(other.active_taxis_);
    locations_.swap(other.locations_);
    removal_cache_.swap(other.removal_cache_);
    empty_taxi_index_.swap(other.empty_taxi_index_);
    swap(benefit_, other.benefit_);
    route_scratch_.swap(other.route_scratch_);
    swap(relocation_check_, other.relocation_check_);
}

Solution SearchState::solution() const {
    Solution result;
    result.routes.resize(instance_->taxis.size());
    for (const auto taxi : active_taxis_) result.routes[taxi] = cache(taxi).nodes();
    return result;
}

const RouteCache& SearchState::cache(std::size_t taxi) const {
    static const RouteCache empty;
    return taxi < routes_by_taxi_.size() ? routes_by_taxi_[taxi] : empty;
}

std::size_t SearchState::served_count() const {
    return std::count_if(locations_.begin(), locations_.end(),
                         [](const auto& location) { return location.taxi >= 0; });
}

double SearchState::revenue(std::size_t request) const {
    const auto n = instance_->passengers.size();
    return request < n ? instance_->passengers.at(request).revenue :
                         instance_->parcels.at(request - n).revenue;
}
std::size_t SearchState::pickup_node(std::size_t request) const {
    const auto n = instance_->passengers.size();
    return request < n ? instance_->passenger_pickup(request) :
                         instance_->parcel_pickup(request - n);
}
std::size_t SearchState::dropoff_node(std::size_t request) const {
    const auto n = instance_->passengers.size();
    return request < n ? instance_->passenger_dropoff(request) :
                         instance_->parcel_dropoff(request - n);
}

void SearchState::rebuild_route(std::size_t taxi, const std::vector<std::size_t>& route) {
    const auto& in = *instance_;
    const auto n = in.passengers.size(), m = in.parcels.size(), k_count = in.taxis.size();
    for (const auto node : cache(taxi).nodes()) {
        const auto request = node < k_count + 2 * n ? (node - k_count) % n :
                             n + (node - k_count - 2 * n) % m;
        locations_[request] = {};
    }
    auto& slot = routes_by_taxi_.at(taxi);
    const bool was_active = is_active(taxi);
    if (route.empty()) {
        if (was_active) {
            slot.clear();
            active_taxis_.erase(std::lower_bound(active_taxis_.begin(),
                                                active_taxis_.end(), taxi));
        }
    } else {
        slot.rebuild(in, taxi, route);
        if (!was_active)
            active_taxis_.insert(std::lower_bound(active_taxis_.begin(),
                                                 active_taxis_.end(), taxi), taxi);
    }
    for (std::size_t p = 0; p < route.size(); ++p) {
        const auto node = route[p];
        const bool passenger = node < k_count + 2 * n;
        const auto request = passenger ? (node - k_count) % n :
                              n + (node - k_count - 2 * n) % m;
        auto& loc = locations_[request];
        loc.taxi = static_cast<int>(taxi);
        if (node == pickup_node(request)) loc.pickup = p;
        else loc.dropoff = p;
    }
}

std::optional<double> SearchState::removal_gain(std::size_t request) const {
    const auto loc = location(request);
    if (loc.taxi < 0) return std::nullopt;
    const auto taxi = static_cast<std::size_t>(loc.taxi);
    const auto& old_cache = cache(taxi);
    auto& entry = removal_cache_[request];
    if (entry.route_version == old_cache.version()) return entry.gain;
    const auto size = old_cache.nodes().size();
    FastRouteCheck candidate(*instance_, taxi, old_cache);
    candidate.range(0, loc.pickup);
    candidate.range(loc.pickup + 1, loc.dropoff);
    candidate.range(loc.dropoff + 1, size);
    entry.route_version = old_cache.version();
    entry.gain = candidate.feasible() ? std::optional<double>(
        removal_savings_cost(*instance_, taxi, old_cache.nodes(),
                             loc.pickup, loc.dropoff) - revenue(request)) : std::nullopt;
    return entry.gain;
}

std::optional<double> SearchState::insertion_gain(std::size_t request,
    std::size_t taxi, std::size_t a, std::size_t b) const {
    if (request >= request_count() || location(request).taxi >= 0 || taxi >= taxi_count())
        return std::nullopt;
    const auto& route_cache = cache(taxi);
    const auto size = route_cache.nodes().size();
    const bool passenger = request < instance_->passengers.size();
    if (a > b || b > size || (passenger && a != b) ||
        !route_cache.boundary_allowed(*instance_, a) ||
        !route_cache.boundary_allowed(*instance_, b)) return std::nullopt;
    if (!passenger && instance_->parcels[request - instance_->passengers.size()].weight >
                          instance_->taxis[taxi].parcel_capacity + 1e-8) return std::nullopt;
    FastRouteCheck candidate(*instance_, taxi, route_cache);
    candidate.range(0, a);
    candidate.node(pickup_node(request));
    if (!passenger) candidate.range(a, b);
    candidate.node(dropoff_node(request));
    candidate.range(b, size);
    if (!candidate.feasible()) return std::nullopt;
    const RouteView view{route_cache.nodes()};
    const auto cost = passenger ? adjacent_insertion_cost(*instance_, taxi, view,
        a, pickup_node(request), dropoff_node(request)) :
        separated_insertion_cost(*instance_, taxi, view, a, b,
            pickup_node(request), dropoff_node(request));
    return revenue(request) - cost;
}

bool SearchState::remove(std::size_t request) {
    const auto gain = removal_gain(request);
    if (!gain) return false;
    const auto loc = location(request);
    const auto taxi = static_cast<std::size_t>(loc.taxi);
    auto& route = route_scratch_;
    route = cache(taxi).nodes();
    route.erase(route.begin() + loc.dropoff);
    route.erase(route.begin() + loc.pickup);
    benefit_ += *gain;
    rebuild_route(taxi, route);
    return true;
}

bool SearchState::insert(std::size_t request, std::size_t taxi,
                         std::size_t a, std::size_t b) {
    const auto gain = insertion_gain(request, taxi, a, b);
    if (!gain) return false;
    auto& route = route_scratch_;
    route = cache(taxi).nodes();
    route.insert(route.begin() + b, dropoff_node(request));
    route.insert(route.begin() + a, pickup_node(request));
    benefit_ += *gain;
    rebuild_route(taxi, route);
    return true;
}

bool SearchState::relocate_within_route(std::size_t request, std::size_t a,
                                       std::size_t b) {
    if (request >= request_count() || location(request).taxi < 0) return false;
    const auto taxi = static_cast<std::size_t>(location(request).taxi);
    const auto old_cost = cache(taxi).cost();
    auto& route = route_scratch_;
    route = cache(taxi).nodes();
    const auto loc = location(request);
    route.erase(route.begin() + loc.dropoff);
    route.erase(route.begin() + loc.pickup);
    const bool passenger = request < passenger_count();
    if (a > b || b > route.size() || (passenger && a != b)) return false;
    const auto boundary_allowed = [&](std::size_t position) {
        if (!position) return true;
        const auto preceding = route[position - 1], k = instance_->taxis.size();
        return preceding < k || preceding >= k + passenger_count();
    };
    if (!boundary_allowed(a) || !boundary_allowed(b)) return false;
    if (!passenger && instance_->parcels[request - passenger_count()].weight >
                          instance_->taxis[taxi].parcel_capacity + 1e-8) return false;
    route.insert(route.begin() + b, dropoff_node(request));
    route.insert(route.begin() + a, pickup_node(request));
    relocation_check_.rebuild(*instance_, taxi, route);
    RouteCandidate candidate(*instance_, taxi, relocation_check_);
    candidate.add_range(0, route.size());
    const auto cost = candidate.cost_if_feasible();
    if (!cost) return false;
    benefit_ += old_cost - *cost;
    rebuild_route(taxi, route);
    return true;
}

void SearchState::check() const {
    const auto result = solution().evaluate(*instance_);
    if (!result.feasible || std::abs(result.benefit - benefit_) >
                               1e-6 * std::max(1.0, std::abs(result.benefit)))
        throw std::logic_error("search-state objective or feasibility mismatch: " + result.error);
}
} // namespace soict::alns

// END src/search/alns/SearchState.cpp

// BEGIN src/search/alns/Balance.cpp

// BEGIN include/search/alns/Balance.hpp
#include <cstddef>
#include <random>
#include <vector>

namespace soict::alns {
struct Choice {
    std::size_t couple = 0, size_index = 0, removal_count = 0;
};
class Balance {
public:
    Balance(std::size_t couples, std::size_t requests, bool thompson);
    Choice select(std::mt19937_64& rng, std::size_t served);
    void update(Choice choice, double reward);
    [[nodiscard]] const std::vector<double>& rewards() const { return reward_sum_; }
    [[nodiscard]] const std::vector<std::size_t>& calls() const { return calls_; }
private:
    struct Arm {
        double weight = 1, sum = 0, square_sum = 0;
        std::size_t count = 0;
    };
    bool thompson_;
    std::vector<Arm> couples_;
    std::vector<std::vector<Arm>> sizes_;
    std::vector<std::size_t> powers_;
    std::vector<double> reward_sum_;
    std::vector<std::size_t> calls_;
    std::size_t choose(const std::vector<Arm>& arms, std::mt19937_64& rng) const;
};
} // namespace soict::alns

// END include/search/alns/Balance.hpp

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace soict::alns {
Balance::Balance(std::size_t couples, std::size_t requests, bool thompson)
    : thompson_(thompson), couples_(couples), sizes_(couples),
      reward_sum_(couples, 0), calls_(couples, 0) {
    if (couples == 0) throw std::invalid_argument("BALANCE needs an operator couple");
    for (std::size_t p = 1; p <= requests; p *= 2) {
        powers_.push_back(p);
        if (p > requests / 2) break;
    }
    if (powers_.empty()) powers_.push_back(1);
    for (auto& arms : sizes_) arms.resize(powers_.size());
}

std::size_t Balance::choose(const std::vector<Arm>& arms, std::mt19937_64& rng) const {
    if (!thompson_) {
        double total = 0;
        for (const auto& arm : arms) total += arm.weight;
        std::uniform_real_distribution<double> draw(0, total);
        double target = draw(rng);
        for (std::size_t i = 0; i < arms.size(); ++i) {
            target -= arms[i].weight;
            if (target <= 0) return i;
        }
        return arms.size() - 1;
    }
    // Normal-Gamma posterior with MV-FSTSP priors.
    constexpr double mu0 = 0, lambda0 = 0.01, alpha0 = 1, beta0 = 100;
    std::size_t choice = 0;
    double best = -std::numeric_limits<double>::infinity();
    for (std::size_t i = 0; i < arms.size(); ++i) {
        const auto& a = arms[i];
        const double count = static_cast<double>(a.count);
        const double lambda = lambda0 + count;
        const double mean = (lambda0 * mu0 + a.sum) / lambda;
        const double alpha = alpha0 + count / 2;
        const double beta = beta0 + 0.5 *
            (a.square_sum + lambda0 * mu0 * mu0 - lambda * mean * mean);
        std::gamma_distribution<double> gamma(alpha, 1.0 / std::max(beta, 1e-12));
        const double precision = gamma(rng);
        std::normal_distribution<double> normal(mean,
            1.0 / std::sqrt(std::max(lambda * precision, 1e-12)));
        const double sample = normal(rng);
        if (sample > best) { best = sample; choice = i; }
    }
    return choice;
}

Choice Balance::select(std::mt19937_64& rng, std::size_t served) {
    Choice choice;
    choice.couple = choose(couples_, rng);
    choice.size_index = choose(sizes_[choice.couple], rng);
    choice.removal_count = std::min(served, powers_[choice.size_index]);
    return choice;
}

void Balance::update(Choice choice, double reward) {
    reward = std::max(0.0, reward);
    auto update_arm = [reward](Arm& a) {
        ++a.count;
        a.sum += reward;
        a.square_sum += reward * reward;
        a.weight = 0.9 * a.weight + 0.1 * (0.01 + reward);
    };
    update_arm(couples_[choice.couple]);
    update_arm(sizes_[choice.couple][choice.size_index]);
    reward_sum_[choice.couple] += reward;
    ++calls_[choice.couple];
}
} // namespace soict::alns

// END src/search/alns/Balance.cpp

// BEGIN src/search/alns/ALNS.cpp

// BEGIN include/search/alns/ALNS.hpp

// BEGIN include/search/alns/Config.hpp

#include <cstdint>
#include <cstddef>
#include <cmath>
#include <string>

namespace soict::alns {
struct Config {
    std::uint64_t seed = 1;
    double time_limit = 59.0;
    double temperature = 10.0;
    double cooling = std::exp(std::log(1e-4) / 60000.0);
    bool check = false;
    bool local_search = true;
    std::size_t ls_interval = 1;
    bool thompson = false;
    std::string log_directory;
};
} // namespace soict::alns

// END include/search/alns/Config.hpp
#include <cstddef>

namespace soict::alns {
struct Result {
    Solution solution;
    double benefit = 0;
    std::size_t iterations = 0;
    double seconds = 0;
    bool annealing_complete = false;
};
Result solve(const Instance& instance, const Config& config);
} // namespace soict::alns

// END include/search/alns/ALNS.hpp

// BEGIN include/search/alns/operators/Destroy/RandomRemoval.hpp

// BEGIN include/search/alns/operators/Destroy/DestroyOperator.hpp
#include <cstddef>
#include <random>
#include <string_view>
#include <vector>

namespace soict::alns {
class DestroyOperator {
public:
    virtual ~DestroyOperator() = default;
    virtual std::string_view name() const = 0;
    virtual std::vector<std::size_t> apply(SearchState& state, std::size_t count,
                                           std::mt19937_64& rng) const = 0;
};
} // namespace soict::alns

// END include/search/alns/operators/Destroy/DestroyOperator.hpp

namespace soict::alns {
class RandomRemoval final : public DestroyOperator {
public:
    std::string_view name() const override { return "random"; }
    std::vector<std::size_t> apply(SearchState&, std::size_t, std::mt19937_64&) const override;
};
} // namespace soict::alns

// END include/search/alns/operators/Destroy/RandomRemoval.hpp

// BEGIN include/search/alns/operators/Destroy/RelatedRemoval.hpp

namespace soict::alns {
class RelatedRemoval final : public DestroyOperator {
public:
    std::string_view name() const override { return "related"; }
    std::vector<std::size_t> apply(SearchState&, std::size_t, std::mt19937_64&) const override;
};
} // namespace soict::alns

// END include/search/alns/operators/Destroy/RelatedRemoval.hpp

// BEGIN include/search/alns/operators/Destroy/WorstRemoval.hpp

namespace soict::alns {
class WorstRemoval final : public DestroyOperator {
public:
    std::string_view name() const override { return "worst"; }
    std::vector<std::size_t> apply(SearchState&, std::size_t, std::mt19937_64&) const override;
};
} // namespace soict::alns

// END include/search/alns/operators/Destroy/WorstRemoval.hpp

// BEGIN include/search/alns/operators/LocalSearch/LocalSearch.hpp

// BEGIN include/search/alns/operators/LocalSearch/Move.hpp

namespace soict::alns {
class Move {
public:
    virtual ~Move() = default;
    virtual std::string_view name() const = 0;
    virtual bool improve(SearchState& state) const = 0;
};
} // namespace soict::alns

// END include/search/alns/operators/LocalSearch/Move.hpp
#include <memory>
#include <vector>

namespace soict::alns {
class LocalSearch {
public:
    void add_move(std::unique_ptr<Move> move) { moves_.push_back(std::move(move)); }
    bool search(SearchState& state) const;
private:
    std::vector<std::unique_ptr<Move>> moves_;
};
} // namespace soict::alns

// END include/search/alns/operators/LocalSearch/LocalSearch.hpp

// BEGIN include/search/alns/operators/LocalSearch/ParcelRelocate.hpp

// BEGIN include/search/alns/operators/LocalSearch/MoveCache.hpp

// BEGIN include/search/alns/operators/Repair/InsertionSearch.hpp

// BEGIN include/search/alns/operators/Repair/RepairOperator.hpp

namespace soict::alns {
class RepairOperator {
public:
    virtual ~RepairOperator() = default;
    virtual std::string_view name() const = 0;
    virtual void apply(SearchState&, const std::vector<std::size_t>& removed,
                       std::mt19937_64&) const = 0;
};
} // namespace soict::alns

// END include/search/alns/operators/Repair/RepairOperator.hpp
#include <chrono>
#include <cstdint>
#include <limits>
#include <optional>
#include <utility>
#include <vector>

namespace soict::alns {
using Deadline = std::chrono::steady_clock::time_point;
struct InsertionChoice {
    std::size_t request = 0, taxi = 0, pickup = 0, delivery = 0;
    double gain = 0;
};
struct InsertionOptions {
    std::optional<InsertionChoice> best;
    double second_gain = -std::numeric_limits<double>::infinity();
};
struct EmptyTaxiChoice {
    std::size_t taxi = 0;
    double gain = 0;
};
class EmptyTaxiIndex {
public:
    explicit EmptyTaxiIndex(const Instance& instance);
    // Extend an unfinished request's index on first use. A solver process
    // searches in one thread, so this shared cache has no locking overhead.
    bool ensure(std::size_t request, Deadline deadline = Deadline::max()) const;
    [[nodiscard]] bool complete(std::size_t request) const {
        return complete_.at(request);
    }
    [[nodiscard]] const std::vector<EmptyTaxiChoice>& choices(std::size_t request) const {
        return choices_.at(request);
    }
private:
    const Instance* instance_;
    mutable std::vector<std::vector<EmptyTaxiChoice>> choices_;
    mutable std::vector<std::size_t> next_taxi_;
    mutable std::vector<bool> complete_;
};
std::shared_ptr<const EmptyTaxiIndex> build_empty_taxi_index(
    const SearchState& empty_state, Deadline deadline);
InsertionOptions insertion_options(const SearchState& state, std::size_t request);
std::optional<InsertionChoice> best_insertion(const SearchState& state,
                                              std::size_t request);

// Reuse an unserved request's route choice while that route is unchanged.
// A scan made above a gain threshold is reused only when the current best
// meets that threshold; otherwise the route is rescanned exactly.
class BestInsertionCache {
public:
    std::optional<InsertionChoice> best(const SearchState& state,
                                        std::size_t request);
private:
    struct Entry {
        std::uint64_t route_version = 0;
        double threshold = -std::numeric_limits<double>::infinity();
        std::optional<InsertionChoice> best;
    };
    std::vector<std::vector<Entry>> entries_;
};

// Repair only inserts requests, so an accepted insertion changes one taxi.
// Retain the top two positions per request and active taxi; rescan that taxi
// after insertion while keeping the other taxis' positions unchanged.
class RepairInsertionCache {
public:
    [[nodiscard]] static bool enabled_for(const SearchState& state);
    explicit RepairInsertionCache(const SearchState& state);
    [[nodiscard]] InsertionOptions options(std::size_t request) const;
    void refresh_taxi(std::size_t taxi);
private:
    const SearchState& state_;
    using TaxiOptions = std::pair<std::size_t, InsertionOptions>;
    std::vector<std::vector<TaxiOptions>> route_options_;
};
} // namespace soict::alns

// END include/search/alns/operators/Repair/InsertionSearch.hpp
#include <cstdint>
#include <optional>
#include <vector>

namespace soict::alns {
// A positive best position on another taxi is also its cheapest feasible
// insertion, so it remains best when only the source route changes. A scan
// finding no positive move proves that fact for any smaller source saving.
// Same-route moves depend on the source route's exact version.
class MoveCache {
public:
    template<class Scan>
    std::optional<InsertionChoice> get(std::size_t request, std::size_t taxi,
        std::size_t taxi_count, std::uint64_t source_version,
        std::uint64_t target_version, double source_saving,
        bool same_route, Scan&& scan) {
        if (entries_.size() <= request) entries_.resize(request + 1);
        auto& row = entries_[request];
        if (row.size() < taxi_count) row.resize(taxi_count);
        auto& entry = row[taxi];
        const bool matching_target = entry.valid &&
            entry.target_version == target_version &&
            entry.same_route == same_route;
        const bool reuse = matching_target && (same_route ?
            entry.source_version == source_version :
            (entry.choice.has_value() || source_saving <= entry.source_saving));
        if (!reuse) {
            entry.choice = scan();
            entry.source_version = source_version;
            entry.target_version = target_version;
            entry.source_saving = source_saving;
            entry.same_route = same_route;
            entry.valid = true;
        }
        if (entry.choice && !same_route) {
            auto adjusted = *entry.choice;
            adjusted.gain += source_saving - entry.source_saving;
            return adjusted;
        }
        return entry.choice;
    }
private:
    struct Entry {
        std::optional<InsertionChoice> choice;
        std::uint64_t source_version = 0, target_version = 0;
        double source_saving = 0;
        bool same_route = false;
        bool valid = false;
    };
    std::vector<std::vector<Entry>> entries_;
};
} // namespace soict::alns

// END include/search/alns/operators/LocalSearch/MoveCache.hpp

// BEGIN include/search/alns/operators/LocalSearch/RelocationPool.hpp

#include <algorithm>
#include <optional>
#include <vector>

namespace soict::alns {
// For a fixed request and destination route, only its best position can win
// best improvement. A route change invalidates every descriptor that reads it.
struct RelocationDescriptor {
    InsertionChoice choice;
    std::size_t source;
};

class RelocationPool {
public:
    void add(std::optional<InsertionChoice> choice, std::size_t source) {
        if (choice && choice->gain > 1e-9) moves_.push_back({*choice, source});
    }
    [[nodiscard]] std::optional<RelocationDescriptor> best() const {
        if (moves_.empty()) return std::nullopt;
        return *std::max_element(moves_.begin(), moves_.end(),
            [](const auto& a, const auto& b) {
                return a.choice.gain < b.choice.gain;
            });
    }
    void erase_affected(std::size_t first, std::size_t second) {
        soict_erase_if(moves_, [&](const auto& move) {
            return move.source == first || move.source == second ||
                   move.choice.taxi == first || move.choice.taxi == second;
        });
    }
    void clear() { moves_.clear(); }
private:
    std::vector<RelocationDescriptor> moves_;
};
} // namespace soict::alns

// END include/search/alns/operators/LocalSearch/RelocationPool.hpp
#include <optional>

namespace soict::alns {
class ParcelRelocationContext {
public:
    ParcelRelocationContext(const SearchState& state, std::size_t request);
    [[nodiscard]] bool valid() const { return served_; }
    [[nodiscard]] bool removal_feasible() const { return removal_gain_.has_value(); }
    [[nodiscard]] double removal_savings() const { return removal_savings_; }
    [[nodiscard]] std::size_t length(std::size_t taxi) const;
    [[nodiscard]] std::optional<double> gain(std::size_t taxi,
        std::size_t pickup_boundary, std::size_t delivery_boundary) const;
    [[nodiscard]] std::optional<double> gain(std::size_t taxi, const RouteCache& route,
        std::size_t pickup_boundary, std::size_t delivery_boundary) const;
    // Benefit delta from changed travel-cost arcs; feasibility is checked by gain().
    [[nodiscard]] std::optional<double> calculate_delta_fast(std::size_t taxi,
        const RouteCache& route, std::size_t pickup_boundary,
        std::size_t delivery_boundary) const;
    [[nodiscard]] std::optional<FastRouteCheck> pickup_prefix(
        std::size_t taxi, const RouteCache& route, std::size_t pickup_boundary) const;
    [[nodiscard]] std::optional<double> gain_from_prefix(std::size_t taxi,
        const RouteCache& route, std::size_t pickup_boundary,
        std::size_t delivery_boundary, const FastRouteCheck& prefix) const;
private:
    const SearchState& state_;
    std::size_t request_, source_, pickup_at_, delivery_at_;
    bool served_ = false;
    std::optional<double> removal_gain_;
    double removal_savings_ = 0;
    Segment pickup_, delivery_;
    [[nodiscard]] std::size_t original_position(std::size_t position) const;
    [[nodiscard]] bool source_boundary_allowed(const RouteCache& route,
                                                std::size_t boundary) const;
    void add_source_range(FastRouteCheck& candidate, std::size_t first,
                          std::size_t end) const;
};

class ParcelRelocate final : public Move {
public:
    std::string_view name() const override { return "parcel_relocate"; }
    bool improve(SearchState&) const override;
private:
    mutable MoveCache cache_;
    [[nodiscard]] std::optional<InsertionChoice> best_on_taxi(
        const SearchState&, const ParcelRelocationContext&, std::size_t request,
        std::size_t taxi, const RouteCache&, std::uint64_t source_version) const;
};
} // namespace soict::alns

// END include/search/alns/operators/LocalSearch/ParcelRelocate.hpp

// BEGIN include/search/alns/operators/LocalSearch/PassengerRelocate.hpp
#include <optional>

namespace soict::alns {
// One source-removal context is shared by every candidate destination. For a
// same-route move, positions refer to the route after the passenger is removed.
class PassengerRelocationContext {
public:
    PassengerRelocationContext(const SearchState& state, std::size_t request);
    [[nodiscard]] bool valid() const { return served_; }
    [[nodiscard]] bool removal_feasible() const { return removal_gain_.has_value(); }
    [[nodiscard]] double removal_savings() const { return removal_savings_; }
    [[nodiscard]] std::size_t length(std::size_t taxi) const;
    [[nodiscard]] std::optional<double> gain(std::size_t taxi,
                                              std::size_t boundary) const;
    [[nodiscard]] std::optional<double> gain(std::size_t taxi,
        const RouteCache& route, std::size_t boundary) const;
    // Benefit delta from changed travel-cost arcs; feasibility is checked by gain().
    [[nodiscard]] std::optional<double> calculate_delta_fast(std::size_t taxi,
        const RouteCache& route, std::size_t boundary) const;
private:
    const SearchState& state_;
    std::size_t request_, source_, removed_at_;
    bool served_ = false;
    std::optional<double> removal_gain_;
    double removal_savings_ = 0;
    [[nodiscard]] std::size_t original_position(std::size_t position) const;
    void add_source_range(FastRouteCheck& candidate, std::size_t first,
                          std::size_t end) const;
};

class PassengerRelocate final : public Move {
public:
    std::string_view name() const override { return "passenger_relocate"; }
    bool improve(SearchState&) const override;
private:
    mutable MoveCache cache_;
    [[nodiscard]] std::optional<InsertionChoice> best_on_taxi(
        const SearchState&, const PassengerRelocationContext&, std::size_t request,
        std::size_t taxi, const RouteCache&, std::uint64_t source_version) const;
};
} // namespace soict::alns

// END include/search/alns/operators/LocalSearch/PassengerRelocate.hpp

// BEGIN include/search/alns/operators/LocalSearch/AddRequest.hpp

namespace soict::alns {
class AddRequest final : public Move {
public:
    std::string_view name() const override { return "add-request"; }
    bool improve(SearchState& state) const override;
private:
    mutable BestInsertionCache cache_;
};
} // namespace soict::alns

// END include/search/alns/operators/LocalSearch/AddRequest.hpp

// BEGIN include/search/alns/operators/LocalSearch/RemoveRequest.hpp

namespace soict::alns {
class RemoveRequest final : public Move {
public:
    std::string_view name() const override { return "remove-request"; }
    bool improve(SearchState& state) const override;
};
} // namespace soict::alns

// END include/search/alns/operators/LocalSearch/RemoveRequest.hpp

// BEGIN include/search/alns/operators/Repair/GreedyInsertion.hpp

namespace soict::alns {
class GreedyInsertion final : public RepairOperator {
public:
    std::string_view name() const override { return "greedy"; }
    void apply(SearchState&, const std::vector<std::size_t>&, std::mt19937_64&) const override;
};
} // namespace soict::alns

// END include/search/alns/operators/Repair/GreedyInsertion.hpp

#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <numeric>
#include <random>
#include <stdexcept>

namespace soict::alns {
Result solve(const Instance& instance, const Config& config) {
    using Clock = std::chrono::steady_clock;
    if (!config.ls_interval) throw std::invalid_argument("ls interval must be positive");
    const auto start = Clock::now();
    const auto deadline = start + std::chrono::duration_cast<Clock::duration>(
                                  std::chrono::duration<double>(config.time_limit));
    std::mt19937_64 rng(config.seed);
    std::vector<std::unique_ptr<DestroyOperator>> destroys;
    destroys.push_back(std::make_unique<RandomRemoval>());
    destroys.push_back(std::make_unique<WorstRemoval>());
    destroys.push_back(std::make_unique<RelatedRemoval>());
    std::vector<std::unique_ptr<RepairOperator>> repairs;
    repairs.push_back(std::make_unique<GreedyInsertion>());
    struct Couple { std::size_t destroy, repair; };
    std::vector<Couple> couples;
    for (std::size_t d = 0; d < destroys.size(); ++d)
        for (std::size_t r = 0; r < repairs.size(); ++r) couples.push_back({d, r});
    Balance balance(couples.size(), instance.passengers.size() + instance.parcels.size(),
                    config.thompson);
    LocalSearch local_search;
    local_search.add_move(std::make_unique<RemoveRequest>());
    local_search.add_move(std::make_unique<AddRequest>());
    local_search.add_move(std::make_unique<PassengerRelocate>());
    local_search.add_move(std::make_unique<ParcelRelocate>());

    Solution empty;
    empty.routes.resize(instance.taxis.size());
    SearchState current(instance, std::move(empty));
    const auto index_deadline = std::min(deadline, Clock::now() +
        std::chrono::duration_cast<Clock::duration>(
            std::chrono::duration<double>(std::min(1.0, config.time_limit * 0.05))));
    current.set_empty_taxi_index(build_empty_taxi_index(current, index_deadline));
    std::vector<std::size_t> initial_order(current.request_count());
    std::iota(initial_order.begin(), initial_order.end(), 0);
    std::shuffle(initial_order.begin(), initial_order.end(), rng);
    for (const auto request : initial_order) {
        if (Clock::now() >= deadline) break;
        const auto choice = best_insertion(current, request);
        if (choice && choice->gain > 1e-9)
            current.insert(request, choice->taxi, choice->pickup, choice->delivery);
    }
    if (config.check) current.check();
    SearchState best = current;
    SearchState candidate = current;
    double temperature = config.temperature;
    const double minimum_temperature = config.temperature * 1e-4;
    std::size_t iterations = 0;
    while (temperature > minimum_temperature) {
        const auto iteration_start = Clock::now();
        if (iteration_start >= deadline) break;
        const auto selection = balance.select(rng, current.served_count());
        const auto couple = couples[selection.couple];
        candidate.copy_from(current);
        const auto removed = destroys[couple.destroy]->apply(
            candidate, selection.removal_count, rng);
        repairs[couple.repair]->apply(candidate, removed, rng);
        if (config.local_search && iterations % config.ls_interval == 0)
            local_search.search(candidate);
        if (config.check) candidate.check();
        const double delta = candidate.benefit() - current.benefit();
        const double reward = std::max(0.0, delta / std::max(1.0, std::abs(current.benefit())));
        bool accept = delta >= 0;
        if (!accept) {
            std::uniform_real_distribution<double> uniform(0, 1);
            accept = uniform(rng) < std::exp(delta / temperature);
        }
        if (accept) {
            current.swap(candidate);
            if (current.benefit() > best.benefit() + 1e-9) {
                best.copy_from(current);

            }
        }
        balance.update(selection, reward);
        ++iterations;
        temperature = std::max(minimum_temperature, temperature * config.cooling);
    }
    if (config.check) best.check();
    Result result;
    result.solution = best.solution();
    result.benefit = best.benefit();
    result.iterations = iterations;
    result.seconds = std::chrono::duration<double>(Clock::now() - start).count();
    result.annealing_complete = temperature <= minimum_temperature;
    return result;
}
} // namespace soict::alns

// END src/search/alns/ALNS.cpp

// BEGIN src/search/alns/operators/Destroy/RandomRemoval.cpp

#include <algorithm>
#include <numeric>

namespace soict::alns {
std::vector<std::size_t> RandomRemoval::apply(SearchState& state, std::size_t count,
                                std::mt19937_64& rng) const {
    std::vector<std::size_t> requests(state.request_count());
    std::iota(requests.begin(), requests.end(), 0);
    std::shuffle(requests.begin(), requests.end(), rng);
    std::vector<std::size_t> removed;
    removed.reserve(count);
    for (const auto request : requests) {
        if (removed.size() >= count) break;
        if (state.remove(request)) removed.push_back(request);
    }
    return removed;
}
} // namespace soict::alns

// END src/search/alns/operators/Destroy/RandomRemoval.cpp

// BEGIN src/search/alns/operators/Destroy/WorstRemoval.cpp

#include <limits>

namespace soict::alns {
std::vector<std::size_t> WorstRemoval::apply(SearchState& state, std::size_t count,
                                std::mt19937_64&) const {
    std::vector<std::size_t> removed;
    removed.reserve(count);
    while (removed.size() < count) {
        std::size_t choice = state.request_count();
        double best_gain = -std::numeric_limits<double>::infinity();
        for (std::size_t r = 0; r < state.request_count(); ++r) {
            const auto gain = state.removal_gain(r);
            if (gain && *gain > best_gain) { choice = r; best_gain = *gain; }
        }
        if (choice == state.request_count() || !state.remove(choice)) break;
        removed.push_back(choice);
    }
    return removed;
}
} // namespace soict::alns

// END src/search/alns/operators/Destroy/WorstRemoval.cpp

// BEGIN src/search/alns/operators/Destroy/RelatedRemoval.cpp

#include <cmath>
#include <limits>
#include <vector>

namespace soict::alns {
std::vector<std::size_t> RelatedRemoval::apply(SearchState& state, std::size_t count,
                                std::mt19937_64& rng) const {
    std::vector<std::size_t> served;
    for (std::size_t r = 0; r < state.request_count(); ++r)
        if (state.location(r).taxi >= 0) served.push_back(r);
    if (served.empty()) return {};
    const auto anchor = served[rng() % served.size()];
    const auto& in = state.instance();
    const auto anchor_node = state.pickup_node(anchor);
    const auto earliest = [&](std::size_t r) {
        return r < in.passengers.size() ? in.passengers[r].pickup.earliest :
                                            in.parcels[r - in.passengers.size()].pickup.earliest;
    };
    const double anchor_time = earliest(anchor);
    // Shaw-style proximity in pickup space and time. Directed travel times
    // are used in both directions; no triangle inequality is assumed.
    std::vector<std::size_t> removed;
    removed.reserve(count);
    while (removed.size() < count) {
        std::size_t choice = state.request_count();
        double best_distance = std::numeric_limits<double>::infinity();
        for (std::size_t r = 0; r < state.request_count(); ++r) {
            if (state.location(r).taxi < 0 || !state.removal_gain(r)) continue;
            const auto node = state.pickup_node(r);
            const double distance = in.time(anchor_node, node) + in.time(node, anchor_node)
                                  + std::abs(anchor_time - earliest(r));
            if (distance < best_distance) { best_distance = distance; choice = r; }
        }
        if (choice == state.request_count() || !state.remove(choice)) break;
        removed.push_back(choice);
    }
    return removed;
}
} // namespace soict::alns

// END src/search/alns/operators/Destroy/RelatedRemoval.cpp

// BEGIN src/search/alns/operators/Repair/InsertionSearch.cpp

#include <algorithm>
#include <chrono>

namespace soict::alns {
EmptyTaxiIndex::EmptyTaxiIndex(const Instance& instance)
    : instance_(&instance),
      choices_(instance.passengers.size() + instance.parcels.size()),
      next_taxi_(choices_.size(), 0), complete_(choices_.size(), false) {}

bool EmptyTaxiIndex::ensure(std::size_t request, Deadline deadline) const {
    if (complete_.at(request)) return true;
    const auto& in = *instance_;
    const auto n = in.passengers.size();
    const auto pickup = request < n ? in.passenger_pickup(request) :
                                      in.parcel_pickup(request - n);
    const auto delivery = request < n ? in.passenger_dropoff(request) :
                                        in.parcel_dropoff(request - n);
    const auto revenue = request < n ? in.passengers[request].revenue :
                                       in.parcels[request - n].revenue;
    const RouteCache empty;
    auto& choices = choices_[request];
    for (auto& taxi = next_taxi_[request]; taxi < in.taxis.size(); ++taxi) {
        if (deadline != Deadline::max() &&
            std::chrono::steady_clock::now() >= deadline) return false;
        RouteCandidate candidate(in, taxi, empty);
        candidate.add_node(pickup);
        candidate.add_node(delivery);
        const auto cost = candidate.cost_if_feasible();
        if (cost) choices.push_back({taxi, revenue - *cost});
    }
    std::sort(choices.begin(), choices.end(), [](const auto& a, const auto& b) {
        return a.gain > b.gain || (a.gain == b.gain && a.taxi < b.taxi);
    });
    complete_[request] = true;
    return true;
}

std::shared_ptr<const EmptyTaxiIndex> build_empty_taxi_index(
    const SearchState& empty_state, Deadline deadline) {
    auto index = std::make_shared<EmptyTaxiIndex>(empty_state.instance());
    for (std::size_t request = 0; request < empty_state.request_count(); ++request) {
        if (!index->ensure(request, deadline)) break;
    }
    return index;
}

namespace {
void consider(InsertionOptions& result, InsertionChoice choice) {
    if (!result.best || choice.gain > result.best->gain ||
        (choice.gain == result.best->gain && choice.taxi < result.best->taxi)) {
        if (result.best) result.second_gain = std::max(result.second_gain, result.best->gain);
        result.best = choice;
    } else result.second_gain = std::max(result.second_gain, choice.gain);
}

void combine(InsertionOptions& result, const InsertionOptions& route) {
    if (route.best) consider(result, *route.best);
    result.second_gain = std::max(result.second_gain, route.second_gain);
}

bool add_empty_taxis(InsertionOptions& result, const SearchState& state,
                     std::size_t request) {
    const auto& index = state.empty_taxi_index();
    const bool indexed = index && index->ensure(request);
    if (indexed) {
        std::size_t found = 0;
        for (const auto& empty : index->choices(request)) {
            if (state.is_active(empty.taxi)) continue;
            consider(result, {request, empty.taxi, 0, 0, empty.gain});
            if (++found == 2) break;
        }
    }
    return indexed;
}

struct RequestSegments {
    bool passenger;
    Segment pickup, delivery;
    double revenue, weight;
};

RequestSegments request_segments(const SearchState& state, std::size_t request) {
    const auto& in = state.instance();
    const auto passenger_count = state.passenger_count();
    const bool passenger = request < passenger_count;
    return {passenger,
            single_node(in, state.pickup_node(request)),
            single_node(in, passenger ? in.passenger_dropoff(request) :
                in.parcel_dropoff(request - passenger_count)),
            state.revenue(request),
            passenger ? 0 : in.parcels[request - passenger_count].weight};
}

InsertionOptions scan_route(const SearchState& state, std::size_t request,
                            std::size_t taxi, const RouteCache& route,
                            const RequestSegments& data,
                            double global_second = -std::numeric_limits<double>::infinity()) {
    InsertionOptions result;
    const auto& in = state.instance();
    if (data.weight > in.taxis[taxi].parcel_capacity + 1e-8) return result;
    const auto length = route.nodes().size();
    const auto pickup = data.pickup.first, delivery = data.delivery.first;
    const auto& nodes = route.nodes();
    // The objective change involves only replaced arcs, even for directed
    // nonmetric costs. Use it to reject candidates before segment joins.
    std::vector<double> pickup_delta(length + 1), delivery_delta(length + 1),
                        adjacent_delta(length + 1);
    for (std::size_t pos = 0; pos <= length; ++pos) {
        const auto previous = pos ? nodes[pos - 1] : taxi;
        const auto old_arc = pos < length ? in.cost(previous, nodes[pos]) : 0.0;
        pickup_delta[pos] = in.cost(previous, pickup) - old_arc +
            (pos < length ? in.cost(pickup, nodes[pos]) : 0.0);
        delivery_delta[pos] = in.cost(previous, delivery) - old_arc +
            (pos < length ? in.cost(delivery, nodes[pos]) : 0.0);
        adjacent_delta[pos] = in.cost(previous, pickup) +
            in.cost(pickup, delivery) - old_arc +
            (pos < length ? in.cost(delivery, nodes[pos]) : 0.0);
    }
    const auto initial_threshold = global_second;
    double minimum_delta = std::numeric_limits<double>::infinity();
    double minimum_pickup = std::numeric_limits<double>::infinity();
    for (std::size_t pos = 0; pos <= length; ++pos) {
        minimum_delta = std::min(minimum_delta, adjacent_delta[pos]);
        if (!data.passenger) {
            minimum_delta = std::min(minimum_delta, minimum_pickup + delivery_delta[pos]);
            minimum_pickup = std::min(minimum_pickup, pickup_delta[pos]);
        }
    }
    if (data.revenue - minimum_delta + 1e-8 < initial_threshold) return result;
    std::vector<double> suffix_delivery;
    if (!data.passenger) {
        suffix_delivery.resize(length + 2, std::numeric_limits<double>::infinity());
        for (std::size_t pos = length + 1; pos-- > 0;)
            suffix_delivery[pos] = std::min(delivery_delta[pos], suffix_delivery[pos + 1]);
    }
    for (std::size_t a = 0; a <= length; ++a) {
        if (!route.boundary_allowed(in, a)) continue;
        const auto threshold = [&] { return std::max(global_second, result.second_gain); };
        const auto best_delta = data.passenger ? adjacent_delta[a] :
            std::min(adjacent_delta[a], pickup_delta[a] + suffix_delivery[a + 1]);
        if (data.revenue - best_delta + 1e-8 < threshold()) continue;
        FastRouteCheck prefix(in, taxi, route);
        prefix.range(0, a);
        prefix.segment(data.pickup);
        if (data.passenger) {
            auto candidate = prefix;
            candidate.segment(data.delivery);
            candidate.range(a, length);
            if (candidate.feasible())
                consider(result, {request, taxi, a, a,
                    data.revenue - adjacent_delta[a]});
        } else {
            for (std::size_t b = a; b <= length; ++b) {
                if (!route.boundary_allowed(in, b)) continue;
                const auto delta = b == a ? adjacent_delta[a] :
                                   pickup_delta[a] + delivery_delta[b];
                if (data.revenue - delta + 1e-8 < threshold()) continue;
                auto candidate = prefix;
                candidate.range(a, b);
                candidate.segment(data.delivery);
                candidate.range(b, length);
                if (candidate.feasible())
                    consider(result, {request, taxi, a, b, data.revenue - delta});
            }
        }
    }
    return result;
}
} // namespace

namespace {
InsertionOptions insertion_options_impl(const SearchState& state, std::size_t request,
                                         bool need_second) {
    InsertionOptions result;
    if (state.location(request).taxi >= 0) return result;
    const bool indexed = add_empty_taxis(result, state, request);
    const auto data = request_segments(state, request);
    const auto threshold = [&] {
        return need_second ? result.second_gain :
            (result.best ? result.best->gain : -std::numeric_limits<double>::infinity());
    };
    if (indexed) {
        for (const auto taxi : state.active_taxis())
            combine(result, scan_route(state, request, taxi, state.cache(taxi), data,
                                       threshold()));
    } else {
        for (std::size_t taxi = 0; taxi < state.taxi_count(); ++taxi)
            combine(result, scan_route(state, request, taxi, state.cache(taxi), data,
                                       threshold()));
    }
    return result;
}
} // namespace

InsertionOptions insertion_options(const SearchState& state, std::size_t request) {
    return insertion_options_impl(state, request, true);
}

std::optional<InsertionChoice> best_insertion(const SearchState& state,
                                              std::size_t request) {
    return insertion_options_impl(state, request, false).best;
}

std::optional<InsertionChoice> BestInsertionCache::best(const SearchState& state,
                                                        std::size_t request) {
    if (state.location(request).taxi >= 0) return std::nullopt;
    InsertionOptions result;
    if (!add_empty_taxis(result, state, request))
        return best_insertion(state, request);
    if (entries_.size() <= request) entries_.resize(request + 1);
    auto& row = entries_[request];
    if (row.size() < state.taxi_count()) row.resize(state.taxi_count());
    const auto data = request_segments(state, request);
    for (const auto taxi : state.active_taxis()) {
        const auto& route = state.cache(taxi);
        auto& entry = row[taxi];
        const auto threshold = result.best ? result.best->gain :
            -std::numeric_limits<double>::infinity();
        if (entry.route_version != route.version() || threshold < entry.threshold) {
            entry.best = scan_route(state, request, taxi, route, data, threshold).best;
            entry.route_version = route.version();
            entry.threshold = threshold;
        }
        if (entry.best) consider(result, *entry.best);
    }
    return result.best;
}

bool RepairInsertionCache::enabled_for(const SearchState& state) {
    return state.empty_taxi_index() && state.request_count() - state.served_count() >= 4;
}

RepairInsertionCache::RepairInsertionCache(const SearchState& state)
    : state_(state), route_options_(state.request_count()) {
    if (!state.empty_taxi_index()) return;
    for (std::size_t request = 0; request < state.request_count(); ++request) {
        if (state.location(request).taxi >= 0) continue;
        const auto data = request_segments(state, request);
        auto& options = route_options_[request];
        options.reserve(state.active_taxis().size());
        for (const auto taxi : state.active_taxis())
            options.emplace_back(taxi, scan_route(state, request, taxi,
                                                 state.cache(taxi), data));
    }
}

InsertionOptions RepairInsertionCache::options(std::size_t request) const {
    InsertionOptions result;
    if (state_.location(request).taxi >= 0) return result;
    if (!state_.empty_taxi_index()) return insertion_options(state_, request);
    add_empty_taxis(result, state_, request);
    for (const auto& [taxi, route] : route_options_.at(request)) combine(result, route);
    return result;
}

void RepairInsertionCache::refresh_taxi(std::size_t taxi) {
    if (!state_.empty_taxi_index()) return;
    const auto& route = state_.cache(taxi);
    for (std::size_t request = 0; request < state_.request_count(); ++request) {
        if (state_.location(request).taxi >= 0) continue;
        auto& options = route_options_[request];
        const auto found = std::lower_bound(options.begin(), options.end(), taxi,
            [](const TaxiOptions& entry, std::size_t id) { return entry.first < id; });
        const auto data = request_segments(state_, request);
        auto updated = scan_route(state_, request, taxi, route, data);
        if (found == options.end() || found->first != taxi)
            options.insert(found, {taxi, std::move(updated)});
        else found->second = std::move(updated);
    }
}
} // namespace soict::alns

// END src/search/alns/operators/Repair/InsertionSearch.cpp

// BEGIN src/search/alns/operators/Repair/GreedyInsertion.cpp

namespace soict::alns {
void GreedyInsertion::apply(SearchState& state, const std::vector<std::size_t>& removed,
                            std::mt19937_64&) const {
    for (const auto request : removed) {
        const auto choice = best_insertion(state, request);
        if (choice && choice->gain > 1e-9)
            state.insert(request, choice->taxi, choice->pickup, choice->delivery);
    }
}
} // namespace soict::alns

// END src/search/alns/operators/Repair/GreedyInsertion.cpp

// BEGIN src/search/alns/operators/Repair/RegretInsertion.cpp

// BEGIN include/search/alns/operators/Repair/RegretInsertion.hpp

namespace soict::alns {
class RegretInsertion final : public RepairOperator {
public:
    std::string_view name() const override { return "regret2"; }
    void apply(SearchState&, const std::vector<std::size_t>&, std::mt19937_64&) const override;
};
} // namespace soict::alns

// END include/search/alns/operators/Repair/RegretInsertion.hpp

#include <cmath>

namespace soict::alns {
void RegretInsertion::apply(SearchState& state, const std::vector<std::size_t>& removed,
                            std::mt19937_64&) const {
    std::vector<bool> pending(state.request_count(), false);
    for (const auto request : removed) pending.at(request) = true;
    while (true) {
        std::optional<InsertionChoice> choice;
        double best_regret = -1;
        for (const auto request : removed) {
            if (!pending[request]) continue;
            const auto options = insertion_options(state, request);
            if (!options.best || options.best->gain <= 1e-9) continue;
            const double regret = std::isfinite(options.second_gain) ?
                                  options.best->gain - options.second_gain :
                                  options.best->gain;
            if (!choice || regret > best_regret ||
                (regret == best_regret && options.best->gain > choice->gain)) {
                choice = options.best;
                best_regret = regret;
            }
        }
        if (!choice || !state.insert(choice->request, choice->taxi,
                                      choice->pickup, choice->delivery)) return;
        pending[choice->request] = false;
    }
}
} // namespace soict::alns

// END src/search/alns/operators/Repair/RegretInsertion.cpp

// BEGIN src/search/alns/operators/LocalSearch/LocalSearch.cpp

namespace soict::alns {
bool LocalSearch::search(SearchState& state) const {
    bool changed = false;
    for (const auto& move : moves_) {
        changed = move->improve(state) || changed;
    }
    return changed;
}
} // namespace soict::alns

// END src/search/alns/operators/LocalSearch/LocalSearch.cpp

// BEGIN src/search/alns/operators/LocalSearch/AddRequest.cpp

#include <optional>
#include <stdexcept>
#include <vector>

namespace soict::alns {
bool AddRequest::improve(SearchState& state) const {
    std::vector<std::optional<InsertionChoice>> moves(state.request_count());
    for (std::size_t request = 0; request < state.request_count(); ++request) {
        if (state.location(request).taxi >= 0) continue;
        moves[request] = cache_.best(state, request);
    }
    bool changed = false;
    for (;;) {
        std::optional<InsertionChoice> best;
        for (const auto& move : moves)
            if (move && move->gain > 1e-9 &&
                (!best || move->gain > best->gain)) best = move;
        if (!best) break;
        if (!state.insert(best->request, best->taxi, best->pickup, best->delivery))
            throw std::logic_error("add-request descriptor disagrees with mutation");
        changed = true;
        moves[best->request].reset();
        // A changed route can improve the best position of any unserved request.
        // BestInsertionCache reuses route entries whose versions are unchanged.
        for (std::size_t request = 0; request < state.request_count(); ++request)
            if (state.location(request).taxi < 0)
                moves[request] = cache_.best(state, request);
    }
    return changed;
}

} // namespace soict::alns

// END src/search/alns/operators/LocalSearch/AddRequest.cpp

// BEGIN src/search/alns/operators/LocalSearch/RemoveRequest.cpp

#include <optional>
#include <stdexcept>
#include <vector>

namespace soict::alns {
bool RemoveRequest::improve(SearchState& state) const {
    std::vector<std::optional<double>> moves(state.request_count());
    for (std::size_t request = 0; request < state.request_count(); ++request)
        moves[request] = state.removal_gain(request);
    bool changed = false;
    for (;;) {
        std::optional<std::size_t> best;
        double best_gain = 1e-9;
        for (std::size_t request = 0; request < state.request_count(); ++request) {
            if (moves[request] && *moves[request] > best_gain) {
                best = request;
                best_gain = *moves[request];
            }
        }
        if (!best) break;
        const auto taxi = static_cast<std::size_t>(state.location(*best).taxi);
        if (!state.remove(*best))
            throw std::logic_error("remove-request descriptor disagrees with mutation");
        changed = true;
        moves[*best].reset();
        for (std::size_t request = 0; request < state.request_count(); ++request)
            if (state.location(request).taxi == static_cast<int>(taxi))
                moves[request] = state.removal_gain(request);
    }
    return changed;
}
} // namespace soict::alns

// END src/search/alns/operators/LocalSearch/RemoveRequest.cpp

// BEGIN src/search/alns/operators/LocalSearch/PassengerRelocate.cpp

#include <algorithm>
#include <stdexcept>

namespace soict::alns {
PassengerRelocationContext::PassengerRelocationContext(const SearchState& state,
                                                       std::size_t request)
    : state_(state), request_(request), source_(0), removed_at_(0) {
    if (request >= state.passenger_count() || state.location(request).taxi < 0) return;
    const auto& loc = state.location(request);
    source_ = static_cast<std::size_t>(loc.taxi);
    removed_at_ = loc.pickup;
    served_ = true;
    removal_gain_ = state.removal_gain(request);
    const auto& route = state.cache(source_);
    removal_savings_ = removal_savings_cost(state.instance(), source_,
                                            route.nodes(), loc.pickup, loc.dropoff);
}

std::size_t PassengerRelocationContext::length(std::size_t taxi) const {
    const auto size = state_.cache(taxi).nodes().size();
    return taxi == source_ ? size - 2 : size;
}

std::size_t PassengerRelocationContext::original_position(std::size_t position) const {
    return position < removed_at_ ? position : position + 2;
}

void PassengerRelocationContext::add_source_range(FastRouteCheck& candidate,
                                                  std::size_t first,
                                                  std::size_t end) const {
    const auto left_end = std::min(end, removed_at_);
    if (first < left_end) candidate.range(first, left_end);
    const auto right_start = std::max(first, removed_at_);
    if (right_start < end) candidate.range(right_start + 2, end + 2);
}

std::optional<double> PassengerRelocationContext::gain(std::size_t taxi,
                                                        std::size_t boundary) const {
    if (taxi >= state_.taxi_count()) return std::nullopt;
    return gain(taxi, state_.cache(taxi), boundary);
}

std::optional<double> PassengerRelocationContext::calculate_delta_fast(std::size_t taxi,
    const RouteCache& route, std::size_t boundary) const {
    if (!valid() || taxi >= state_.taxi_count() ||
        boundary > route.nodes().size() - (taxi == source_ ? 2 : 0) ||
        (taxi != source_ && !removal_gain_)) return std::nullopt;
    const RouteView view{route.nodes(), removed_at_, removed_at_ + 1, taxi == source_};
    const auto delta = adjacent_insertion_cost(state_.instance(), taxi, view, boundary,
        state_.pickup_node(request_), state_.instance().passenger_dropoff(request_));
    return removal_savings_ - delta;
}

std::optional<double> PassengerRelocationContext::gain(std::size_t taxi,
    const RouteCache& route, std::size_t boundary) const {
    if (!valid() || taxi >= state_.taxi_count() ||
        boundary > route.nodes().size() - (taxi == source_ ? 2 : 0))
        return std::nullopt;
    const auto& in = state_.instance();
    const bool same = taxi == source_;
    const auto route_length = route.nodes().size() - (same ? 2 : 0);
    if (same) {
        if (boundary && !route.boundary_allowed(in, original_position(boundary - 1) + 1))
            return std::nullopt;
    } else if (!removal_gain_ || !route.boundary_allowed(in, boundary)) return std::nullopt;

    FastRouteCheck candidate(in, taxi, route);
    if (same) add_source_range(candidate, 0, boundary);
    else candidate.range(0, boundary);
    candidate.node(state_.pickup_node(request_));
    candidate.node(in.passenger_dropoff(request_));
    if (same) add_source_range(candidate, boundary, route_length);
    else candidate.range(boundary, route_length);
    if (!candidate.feasible()) return std::nullopt;
    return calculate_delta_fast(taxi, route, boundary);
}

std::optional<InsertionChoice> PassengerRelocate::best_on_taxi(
    const SearchState& state, const PassengerRelocationContext& context,
    std::size_t r, std::size_t taxi, const RouteCache& route,
    std::uint64_t source_version) const {
    const auto source = static_cast<std::size_t>(state.location(r).taxi);
    if (taxi != source && !context.removal_feasible()) return std::nullopt;
    return cache_.get(r, taxi, state.taxi_count(),
        source_version, route.version(),
        context.removal_savings(), taxi == source, [&] {
                std::optional<InsertionChoice> route_best;
                const auto length = route.nodes().size() - (taxi == source ? 2 : 0);
                for (std::size_t a = 0; a <= length; ++a) {
                    const auto objective = context.calculate_delta_fast(taxi, route, a);
                    if (!objective || *objective + 1e-8 <
                        std::max(1e-9, route_best ? route_best->gain : 1e-9)) continue;
                    const auto gain = context.gain(taxi, route, a);
                    if (gain && (!route_best || *gain > route_best->gain))
                        route_best = InsertionChoice{r, taxi, a, a, *gain};
                }
                return route_best;
            });
}

bool PassengerRelocate::improve(SearchState& state) const {
    RelocationPool pool;
    const auto scan_request = [&](std::size_t r, bool all,
                                  std::size_t first, std::size_t second) {
        if (state.location(r).taxi < 0) return;
        PassengerRelocationContext context(state, r);
        const auto source = static_cast<std::size_t>(state.location(r).taxi);
        if (!context.valid()) return;
        // The source stays unchanged throughout this request's destination scan.
        const auto source_version = state.cache(source).version();
        const auto& index = state.empty_taxi_index();
        const bool indexed = index && index->ensure(r);
        if (indexed) {
            if (all) {
                static const RouteCache empty_route;
                for (const auto& choice : index->choices(r)) {
                    if (state.is_active(choice.taxi)) continue;
                    const auto gain = context.gain(choice.taxi, empty_route, 0);
                    if (gain) pool.add(InsertionChoice{r, choice.taxi, 0, 0, *gain}, source);
                    break;
                }
            }
            for (const auto taxi : state.active_taxis())
                if (all || taxi == first || taxi == second)
                    pool.add(best_on_taxi(state, context, r, taxi, state.cache(taxi),
                                         source_version), source);
        } else {
            for (std::size_t taxi = 0; taxi < state.taxi_count(); ++taxi)
                if (all || taxi == first || taxi == second)
                    pool.add(best_on_taxi(state, context, r, taxi, state.cache(taxi),
                                         source_version), source);
        }
    };
    const auto scan_all = [&] {
        pool.clear();
        for (std::size_t r = 0; r < state.passenger_count(); ++r)
            scan_request(r, true, 0, 0);
    };
    scan_all();
    bool changed = false;
    while (const auto descriptor = pool.best()) {
        const auto choice = descriptor->choice;
        const auto source = descriptor->source;
        const auto target = choice.taxi;
        const bool source_active_before = state.is_active(source);
        const bool target_active_before = state.is_active(target);
        const bool same = source == target;
        const bool applied = same ? state.relocate_within_route(
            choice.request, choice.pickup, choice.delivery) :
            (state.remove(choice.request) && state.insert(choice.request,
                target, choice.pickup, choice.delivery));
        if (!applied)
            throw std::logic_error("passenger relocation descriptor disagrees with mutation");
        changed = true;
        if (state.is_active(source) != source_active_before ||
            state.is_active(target) != target_active_before) {
            scan_all();
            continue;
        }
        pool.erase_affected(source, target);
        for (std::size_t r = 0; r < state.passenger_count(); ++r) {
            if (state.location(r).taxi < 0) continue;
            const auto origin = static_cast<std::size_t>(state.location(r).taxi);
            scan_request(r, origin == source || origin == target, source, target);
        }
    }
    return changed;
}

} // namespace soict::alns

// END src/search/alns/operators/LocalSearch/PassengerRelocate.cpp

// BEGIN src/search/alns/operators/LocalSearch/ParcelRelocate.cpp

#include <algorithm>
#include <limits>
#include <stdexcept>

namespace soict::alns {
ParcelRelocationContext::ParcelRelocationContext(const SearchState& state,
                                                 std::size_t request)
    : state_(state), request_(request), source_(0), pickup_at_(0), delivery_at_(0) {
    if (request < state.passenger_count() || request >= state.request_count() ||
        state.location(request).taxi < 0) return;
    const auto& loc = state.location(request);
    source_ = static_cast<std::size_t>(loc.taxi);
    pickup_at_ = loc.pickup;
    delivery_at_ = loc.dropoff;
    served_ = true;
    removal_gain_ = state.removal_gain(request);
    pickup_ = single_node(state.instance(), state.pickup_node(request));
    delivery_ = single_node(state.instance(),
        state.instance().parcel_dropoff(request - state.passenger_count()));
    const auto& route = state.cache(source_);
    removal_savings_ = removal_savings_cost(state.instance(), source_,
                                            route.nodes(), loc.pickup, loc.dropoff);
}

std::size_t ParcelRelocationContext::length(std::size_t taxi) const {
    const auto size = state_.cache(taxi).nodes().size();
    return taxi == source_ ? size - 2 : size;
}

std::size_t ParcelRelocationContext::original_position(std::size_t position) const {
    if (position < pickup_at_) return position;
    if (position < delivery_at_ - 1) return position + 1;
    return position + 2;
}

bool ParcelRelocationContext::source_boundary_allowed(const RouteCache& route,
                                                       std::size_t boundary) const {
    return !boundary || route.boundary_allowed(state_.instance(),
                                               original_position(boundary - 1) + 1);
}

void ParcelRelocationContext::add_source_range(FastRouteCheck& candidate,
                                               std::size_t first,
                                               std::size_t end) const {
    const auto left_end = std::min(end, pickup_at_);
    if (first < left_end) candidate.range(first, left_end);
    const auto middle_start = std::max(first, pickup_at_);
    const auto middle_end = std::min(end, delivery_at_ - 1);
    if (middle_start < middle_end)
        candidate.range(middle_start + 1, middle_end + 1);
    const auto right_start = std::max(first, delivery_at_ - 1);
    if (right_start < end) candidate.range(right_start + 2, end + 2);
}

std::optional<double> ParcelRelocationContext::gain(std::size_t taxi,
    std::size_t a, std::size_t b) const {
    if (taxi >= state_.taxi_count()) return std::nullopt;
    return gain(taxi, state_.cache(taxi), a, b);
}

std::optional<double> ParcelRelocationContext::calculate_delta_fast(std::size_t taxi,
    const RouteCache& route, std::size_t a, std::size_t b) const {
    if (!valid() || taxi >= state_.taxi_count() || a > b ||
        b > route.nodes().size() - (taxi == source_ ? 2 : 0) ||
        (taxi != source_ && !removal_gain_)) return std::nullopt;
    const RouteView view{route.nodes(), pickup_at_, delivery_at_, taxi == source_};
    const auto delta = separated_insertion_cost(state_.instance(), taxi, view, a, b,
        state_.pickup_node(request_),
        state_.instance().parcel_dropoff(request_ - state_.passenger_count()));
    return removal_savings_ - delta;
}

std::optional<double> ParcelRelocationContext::gain(std::size_t taxi,
    const RouteCache& route, std::size_t a, std::size_t b) const {
    const auto prefix = pickup_prefix(taxi, route, a);
    return prefix ? gain_from_prefix(taxi, route, a, b, *prefix) : std::nullopt;
}

std::optional<FastRouteCheck> ParcelRelocationContext::pickup_prefix(
    std::size_t taxi, const RouteCache& route, std::size_t a) const {
    if (!valid() || taxi >= state_.taxi_count()) return std::nullopt;
    const bool same = taxi == source_;
    const auto route_length = route.nodes().size() - (same ? 2 : 0);
    if (a > route_length) return std::nullopt;
    const auto& in = state_.instance();
    const auto weight = in.parcels[request_ - state_.passenger_count()].weight;
    if (weight > in.taxis[taxi].parcel_capacity + 1e-8) return std::nullopt;
    if (same) {
        if (!source_boundary_allowed(route, a)) return std::nullopt;
    } else if (!removal_gain_ || !route.boundary_allowed(in, a)) return std::nullopt;

    FastRouteCheck candidate(in, taxi, route);
    if (same) add_source_range(candidate, 0, a);
    else candidate.range(0, a);
    candidate.segment(pickup_);
    return candidate;
}

std::optional<double> ParcelRelocationContext::gain_from_prefix(
    std::size_t taxi, const RouteCache& route, std::size_t a, std::size_t b,
    const FastRouteCheck& prefix) const {
    const bool same = taxi == source_;
    const auto route_length = route.nodes().size() - (same ? 2 : 0);
    if (a > b || b > route_length) return std::nullopt;
    const auto& in = state_.instance();
    if (same) {
        if (!source_boundary_allowed(route, b)) return std::nullopt;
    } else if (!route.boundary_allowed(in, b)) return std::nullopt;
    auto candidate = prefix;
    if (same) add_source_range(candidate, a, b);
    else candidate.range(a, b);
    candidate.segment(delivery_);
    if (same) add_source_range(candidate, b, route_length);
    else candidate.range(b, route_length);
    if (!candidate.feasible()) return std::nullopt;
    return calculate_delta_fast(taxi, route, a, b);
}

std::optional<InsertionChoice> ParcelRelocate::best_on_taxi(
    const SearchState& state, const ParcelRelocationContext& context,
    std::size_t r, std::size_t taxi, const RouteCache& route,
    std::uint64_t source_version) const {
    const auto source = static_cast<std::size_t>(state.location(r).taxi);
    if (taxi != source && !context.removal_feasible()) return std::nullopt;
    return cache_.get(r, taxi, state.taxi_count(),
        source_version, route.version(),
        context.removal_savings(), taxi == source, [&] {
                std::optional<InsertionChoice> route_best;
                const auto length = route.nodes().size() - (taxi == source ? 2 : 0);
                const auto& in = state.instance();
                const auto loc = state.location(r);
                const RouteView view{route.nodes(), loc.pickup, loc.dropoff,
                                     taxi == source};
                const auto pickup_node = state.pickup_node(r);
                const auto delivery_node = in.parcel_dropoff(r - state.passenger_count());
                std::vector<double> pickup_cost(length + 1), delivery_cost(length + 1),
                                    adjacent_cost(length + 1),
                                    suffix_delivery(length + 2,
                                        std::numeric_limits<double>::infinity());
                for (std::size_t pos = 0; pos <= length; ++pos) {
                    pickup_cost[pos] = single_insertion_cost(in, taxi, view,
                                                             pos, pickup_node);
                    delivery_cost[pos] = single_insertion_cost(in, taxi, view,
                                                               pos, delivery_node);
                    adjacent_cost[pos] = adjacent_insertion_cost(in, taxi, view,
                                             pos, pickup_node, delivery_node);
                }
                for (std::size_t pos = length + 1; pos-- > 0;)
                    suffix_delivery[pos] = std::min(delivery_cost[pos],
                                                    suffix_delivery[pos + 1]);
                for (std::size_t a = 0; a <= length; ++a) {
                    const auto best_delta = std::min(adjacent_cost[a],
                        pickup_cost[a] + suffix_delivery[a + 1]);
                    if (context.removal_savings() - best_delta + 1e-8 <
                        std::max(1e-9, route_best ? route_best->gain : 1e-9)) continue;
                    const auto prefix = context.pickup_prefix(taxi, route, a);
                    if (!prefix) continue;
                    for (std::size_t b = a; b <= length; ++b) {
                        const auto delta = b == a ? adjacent_cost[a] :
                                           pickup_cost[a] + delivery_cost[b];
                        const auto objective = context.removal_savings() - delta;
                        if (objective + 1e-8 <
                            std::max(1e-9, route_best ? route_best->gain : 1e-9)) continue;
                        const auto gain = context.gain_from_prefix(taxi, route, a, b, *prefix);
                        if (gain && (!route_best || *gain > route_best->gain))
                            route_best = InsertionChoice{r, taxi, a, b, *gain};
                    }
                }
                return route_best;
            });
}

bool ParcelRelocate::improve(SearchState& state) const {
    RelocationPool pool;
    const auto scan_request = [&](std::size_t r, bool all,
                                  std::size_t first, std::size_t second) {
        if (state.location(r).taxi < 0) return;
        ParcelRelocationContext context(state, r);
        const auto source = static_cast<std::size_t>(state.location(r).taxi);
        if (!context.valid()) return;
        // The source stays unchanged throughout this request's destination scan.
        const auto source_version = state.cache(source).version();
        const auto& index = state.empty_taxi_index();
        const bool indexed = index && index->ensure(r);
        if (indexed) {
            if (all) {
                static const RouteCache empty_route;
                for (const auto& choice : index->choices(r)) {
                    if (state.is_active(choice.taxi)) continue;
                    const auto gain = context.gain(choice.taxi, empty_route, 0, 0);
                    if (gain) pool.add(InsertionChoice{r, choice.taxi, 0, 0, *gain}, source);
                    break;
                }
            }
            for (const auto taxi : state.active_taxis())
                if (all || taxi == first || taxi == second)
                    pool.add(best_on_taxi(state, context, r, taxi, state.cache(taxi),
                                         source_version), source);
        } else {
            for (std::size_t taxi = 0; taxi < state.taxi_count(); ++taxi)
                if (all || taxi == first || taxi == second)
                    pool.add(best_on_taxi(state, context, r, taxi, state.cache(taxi),
                                         source_version), source);
        }
    };
    const auto scan_all = [&] {
        pool.clear();
        for (std::size_t r = state.passenger_count(); r < state.request_count(); ++r)
            scan_request(r, true, 0, 0);
    };
    scan_all();
    bool changed = false;
    while (const auto descriptor = pool.best()) {
        const auto choice = descriptor->choice;
        const auto source = descriptor->source;
        const auto target = choice.taxi;
        const bool source_active_before = state.is_active(source);
        const bool target_active_before = state.is_active(target);
        const bool same = source == target;
        const bool applied = same ? state.relocate_within_route(
            choice.request, choice.pickup, choice.delivery) :
            (state.remove(choice.request) && state.insert(choice.request,
                target, choice.pickup, choice.delivery));
        if (!applied)
            throw std::logic_error("parcel relocation descriptor disagrees with mutation");
        changed = true;
        if (state.is_active(source) != source_active_before ||
            state.is_active(target) != target_active_before) {
            scan_all();
            continue;
        }
        pool.erase_affected(source, target);
        for (std::size_t r = state.passenger_count(); r < state.request_count(); ++r) {
            if (state.location(r).taxi < 0) continue;
            const auto origin = static_cast<std::size_t>(state.location(r).taxi);
            scan_request(r, origin == source || origin == target, source, target);
        }
    }
    return changed;
}

} // namespace soict::alns

// END src/search/alns/operators/LocalSearch/ParcelRelocate.cpp
#include <iostream>
#include <chrono>

// Judge entry point. Include input parsing in the five-minute wall budget.
int main() {
    const auto started = std::chrono::steady_clock::now();
    try {
        std::ios::sync_with_stdio(false);
        std::cin.tie(nullptr);
        const auto instance = soict::read_instance(std::cin);
        soict::alns::Config config;
        config.seed = 1ULL;
        const double elapsed = std::chrono::duration<double>(
            std::chrono::steady_clock::now() - started).count();
        const double allowance = 300;
        const double reserve = allowance > 2 ? 1.0 : allowance * 0.1;
        config.time_limit = allowance - elapsed - reserve;
        if (config.time_limit <= 0) {
            soict::Solution empty;
            empty.routes.resize(instance.taxis.size());
            soict::write_solution(std::cout, empty, instance);
        } else {
            const auto result = soict::alns::solve(instance, config);
            soict::write_solution(std::cout, result.solution, instance);
        }
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "error: " << error.what() << '\n';
        return 1;
    }
}
