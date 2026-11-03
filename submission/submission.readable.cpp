#pragma GCC optimize("O3","unroll-loops")
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

// BEGIN src/model/Solution.cpp

// BEGIN include/model/Solution.hpp


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

    std::size_t node_count() const {
        return taxis.size() + 2 * (passengers.size() + parcels.size());
    }
    std::size_t vertex_id(std::size_t node) const {
        return vertex_ids.empty() ? node + 1 : vertex_ids[node];
    }
    std::size_t passenger_pickup(std::size_t i) const {
        return taxis.size() + i;
    }
    std::size_t passenger_dropoff(std::size_t i) const {
        return taxis.size() + passengers.size() + i;
    }
    std::size_t parcel_pickup(std::size_t j) const {
        return taxis.size() + 2 * passengers.size() + j;
    }
    std::size_t parcel_dropoff(std::size_t j) const {
        return taxis.size() + 2 * passengers.size() + parcels.size() + j;
    }
    double time(std::size_t u, std::size_t v) const {
        return travel_time[u * node_count() + v];
    }
    double cost(std::size_t u, std::size_t v) const {
        return travel_cost[u * node_count() + v];
    }
};

} // namespace soict

// END include/model/Instance.hpp
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

    Evaluation evaluate(const Instance& instance) const;
};

} // namespace soict

// END include/model/Solution.hpp
namespace soict {
Evaluation Solution::evaluate(const Instance& instance) const {
    const auto n = instance.passengers.size(), m = instance.parcels.size();
    const auto k_count = instance.taxis.size();
    std::vector<bool> served(n + m, false);
    double benefit = 0;
    for (std::size_t k = 0; k < k_count; ++k) {
        std::size_t previous = k;
        for (const auto node : routes[k]) {
            benefit -= instance.cost(previous, node);
            if (node < k_count + n) served[node - k_count] = true;
            else if (node >= k_count + 2 * n && node < k_count + 2 * n + m)
                served[n + node - k_count - 2 * n] = true;
            previous = node;
        }
    }
    for (std::size_t i = 0; i < n + m; ++i)
        if (served[i]) benefit += i < n ? instance.passengers[i].revenue :
                                        instance.parcels[i - n].revenue;
    return {true, benefit, {}};
}
} // namespace soict

// END src/model/Solution.cpp

// BEGIN src/io/InstanceIO.cpp
#include <cstdlib>

// BEGIN include/io/InstanceIO.hpp

#include <iosfwd>

namespace soict {
Instance read_instance(std::istream& in);
void write_instance(std::ostream& out, const Instance& instance);
}

// END include/io/InstanceIO.hpp

#include <array>
#include <charconv>
#include <iomanip>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace soict {
namespace {
// Bounded streaming storage: matrix parsing never retains the input file.
class NumericReader {
    std::istream& input_;
    std::array<char, 65536> buffer_{};
    std::size_t position_ = 0, size_ = 0;
    std::string continued_;
    bool refill() {
        size_ = static_cast<std::size_t>(input_.rdbuf()->sgetn(buffer_.data(), buffer_.size()));
        position_ = 0;
        return size_ != 0;
    }
public:
    explicit NumericReader(std::istream& input) : input_(input) {}
    std::string_view next() {
        continued_.clear();
        for (;;) {
            if (position_ == size_ && !refill()) return {};
            while (position_ < size_ && static_cast<unsigned char>(buffer_[position_]) <= ' ') ++position_;
            if (position_ < size_) break;
        }
        for (;;) {
            const auto start = position_;
            while (position_ < size_ && static_cast<unsigned char>(buffer_[position_]) > ' ') ++position_;
            const std::string_view piece(buffer_.data()+start,position_-start);
            if (position_ < size_) {
                if (continued_.empty()) return piece;
                continued_.append(piece.data(),piece.size());
                return continued_;
            }
            continued_.append(piece.data(),piece.size());
            if (!refill()) return continued_;
        }
    }
};
template <typename T> void read(NumericReader& input, T& value) {
    auto token = input.next();
    // from_chars accepts exponent notation but omits the optional leading +.
    if (!token.empty() && token.front() == '+') token.remove_prefix(1);

    std::from_chars(token.data(),token.data()+token.size(),value);

}
void read(NumericReader& input, double& value) {
    value = std::strtod(std::string(input.next()).c_str(), nullptr);
}
}

Instance read_instance(std::istream& in) {
    NumericReader input(in);
    std::size_t k = 0, n = 0, m = 0;
    read(input, k);
    read(input, n);
    read(input, m);

    const std::size_t v = k + 2 * (n + m);

    Instance instance;
    instance.taxis.resize(k);
    instance.passengers.resize(n);
    instance.parcels.resize(m);
    instance.vertex_ids.resize(v);
    const auto vertex = [&](std::size_t node) {
        auto& id = instance.vertex_ids[node];
        read(input, id);

    };
    for (std::size_t taxi = 0; taxi < k; ++taxi) {
        vertex(taxi);
        read(input, instance.taxis[taxi].parcel_capacity);
    }
    for (std::size_t i = 0; i < n; ++i) {
        auto& p = instance.passengers[i];
        vertex(instance.passenger_pickup(i));
        vertex(instance.passenger_dropoff(i));
        read(input, p.pickup.earliest);
        read(input, p.pickup.latest);
        read(input, p.service);
        read(input, p.revenue);
    }
    for (std::size_t j = 0; j < m; ++j) {
        auto& p = instance.parcels[j];
        vertex(instance.parcel_pickup(j));
        vertex(instance.parcel_dropoff(j));
        read(input, p.weight);
        read(input, p.pickup.earliest);
        read(input, p.pickup.latest);
        read(input, p.delivery.earliest);
        read(input, p.delivery.latest);
        read(input, p.service);
        read(input, p.revenue);
    }
    std::vector<std::size_t> internal(v);
    for (std::size_t node = 0; node < v; ++node)
        internal[instance.vertex_id(node) - 1] = node;
    for (auto* matrix : {&instance.travel_time, &instance.travel_cost}) {
        matrix->resize(v * v);
        for (std::size_t u = 0; u < v; ++u)
            for (std::size_t w = 0; w < v; ++w)
                read(input, (*matrix)[internal[u] * v + internal[w]]);
    }
    // Generator metadata may follow the matrices; it is not part of the instance.
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
    const auto evaluation = solution.evaluate(instance);

    out << std::setprecision(17) << evaluation.benefit << '\n';
    for (const auto& route : solution.routes) {
        out << route.size();
        for (const auto node : route) out << ' ' << instance.vertex_id(node);
        out << '\n';
    }

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

    Segment range(const Instance& instance, std::size_t first,
                                std::size_t last) const; // inclusive
    bool boundary_allowed(const Instance& instance,
                                        std::size_t position) const;
    double cost() const { return cost_; }
    std::uint64_t version() const { return version_; }
    const std::vector<std::size_t>& nodes() const { return nodes_; }

private:
    std::vector<std::size_t> nodes_;
    std::vector<Segment> singles_;
    std::vector<std::vector<Segment>> table_;
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
    std::optional<double> cost_if_feasible() const;

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
    // Even an event without a time window requires a finite arrival. The
    // final departure after service may overflow when no later event exists.
    s.latest_input = std::numeric_limits<double>::max();
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
    for (auto& level : table_) level.clear();
    cost_ = 0;
    version_ = 0;
}

void RouteCache::copy_from(const RouteCache& other) {
    if (this == &other) return;
    nodes_ = other.nodes_;
    singles_ = other.singles_;
    if (table_.size() < other.table_.size()) table_.resize(other.table_.size());
    for (std::size_t i = 0; i < other.table_.size(); ++i) table_[i] = other.table_[i];
    for (std::size_t i = other.table_.size(); i < table_.size(); ++i) table_[i].clear();
    cost_ = other.cost_;
    version_ = other.version_;
}

void RouteCache::rebuild(const Instance& in, std::size_t taxi,
                          const std::vector<std::size_t>& route) {
    if (&route != &nodes_) nodes_ = route;
    singles_.clear(); for (auto& level : table_) level.clear();
    cost_ = 0;
    version_ = next_route_version.fetch_add(1, std::memory_order_relaxed);
    const auto size = route.size();
    singles_.reserve(size);
    std::size_t previous = taxi;
    for (const auto node : route) {
        singles_.push_back(single_node(in, node));
        cost_ += in.cost(previous, node);
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

    if (first == last) return singles_[first];
    const auto level = soict_bit_width(first ^ last) - 1;
    return join(in, table_[level][first], table_[level][last]);
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
class InsertionScratch;

// Request ids 0..N-1 are passengers; N..N+M-1 are parcels.
struct RequestLocation {
    int taxi = -1;
    std::size_t pickup = 0, dropoff = 0;
};

class SearchState {
public:
    SearchState(const Instance& instance, Solution solution,
                std::shared_ptr<InsertionScratch> insertion_scratch = {});
    void copy_from(const SearchState& other);
    Solution solution() const;
    const RouteCache& cache(std::size_t taxi) const;
    const std::vector<std::size_t>& active_taxis() const {
        return active_taxis_;
    }
    bool is_active(std::size_t taxi) const {
        return taxi < routes_by_taxi_.size() && !routes_by_taxi_[taxi].nodes().empty();
    }
    std::size_t taxi_count() const { return instance_->taxis.size(); }
    const RequestLocation& location(std::size_t request) const {
        return locations_[request];
    }
    std::size_t request_count() const { return locations_.size(); }
    std::size_t passenger_count() const { return instance_->passengers.size(); }
    std::size_t served_count() const;
    double benefit() const { return benefit_; }
    // State copies share this workspace; insertion scans within a solve are
    // sequential. Scratch contents are not part of the solution or its caches.
    InsertionScratch& insertion_scratch() const {
        return *insertion_scratch_;
    }
    const std::shared_ptr<InsertionScratch>& insertion_scratch_handle() const {
        return insertion_scratch_;
    }
    double revenue(std::size_t request) const;
    const Instance& instance() const { return *instance_; }
    std::size_t pickup_node(std::size_t request) const;
    const std::shared_ptr<const EmptyTaxiIndex>& empty_taxi_index() const {
        return empty_taxi_index_;
    }
    void set_empty_taxi_index(std::shared_ptr<const EmptyTaxiIndex> index) {
        empty_taxi_index_ = std::move(index);
    }
    std::optional<double> removal_gain(std::size_t request) const;
    std::optional<double> insertion_gain(std::size_t request,
        std::size_t taxi, std::size_t pickup_boundary,
        std::size_t delivery_boundary) const;

    bool remove(std::size_t request);
    // Remove complete requests from one taxi atomically. Intermediate shortcut
    // routes need not be feasible on directed/nonmetric travel times.
    bool remove_requests(std::size_t taxi, const std::vector<std::size_t>& requests);
    bool insert(std::size_t request, std::size_t taxi,
                std::size_t pickup_boundary, std::size_t delivery_boundary);
    bool relocate_within_route(std::size_t request, std::size_t pickup_boundary,
                               std::size_t delivery_boundary);
    // Positions refer to the route after removing outgoing's two endpoints.
    // Only the completed replacement must be feasible and improve benefit.
    bool replace_request(std::size_t outgoing, std::size_t incoming,
                         std::size_t pickup_boundary, std::size_t delivery_boundary);
    // Exchange served requests across taxis, keeping passenger endpoints adjacent.
    bool swap_requests(std::size_t first, std::size_t second);
    // Reject invalid/non-improving exchanges without modifying either route.
    bool exchange_route_suffixes(std::size_t taxi_a, std::size_t cut_a,
                                 std::size_t taxi_b, std::size_t cut_b);

private:
    const Instance* instance_;
    std::shared_ptr<InsertionScratch> insertion_scratch_;
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
    void commit_validated_route(std::size_t taxi, RouteCache& updated, double gain);
    void commit_route_pair(std::size_t taxi_a, RouteCache new_a,
                           std::size_t taxi_b, RouteCache new_b, double gain);
    std::size_t dropoff_node(std::size_t request) const;
};
} // namespace soict::alns

// END include/search/alns/SearchState.hpp

// BEGIN include/search/alns/InsertionScratch.hpp

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

namespace soict::alns {
// Sequential insertion scans share one fixed allocation for the whole solve.
// A route contains at most two service nodes per request. The extra two slots
// cover every insertion gap and the parcel suffix's infinity sentinel.
class InsertionScratch {
public:
    struct Buffers {
        double* pickup;
        double* delivery;
        double* adjacent;
        double* suffix;
    };

    explicit InsertionScratch(std::size_t requests)
        : stride_(checked_stride(requests)), storage_(4 * stride_) {}
    std::size_t max_route_nodes() const { return stride_ - 2; }

    Buffers buffers(std::size_t route_nodes) {

        auto* data = storage_.data();
        return {data, data + stride_, data + 2 * stride_, data + 3 * stride_};
    }

private:
    static std::size_t checked_stride(std::size_t requests) {

        return 2 * requests + 2;
    }
    std::size_t stride_;
    // Never resized: scan results own their values and do not retain pointers.
    std::vector<double> storage_;
};
} // namespace soict::alns

// END include/search/alns/InsertionScratch.hpp

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

    bool feasible() const {
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

    std::size_t size() const { return nodes.size() - (removed ? 2 : 0); }
    std::size_t node(std::size_t position) const {
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
SearchState::SearchState(const Instance& in, Solution solution,
                         std::shared_ptr<InsertionScratch> insertion_scratch)
    : instance_(&in), insertion_scratch_(insertion_scratch ? std::move(insertion_scratch) :
          std::make_shared<InsertionScratch>(in.passengers.size() + in.parcels.size())),
      routes_by_taxi_(in.taxis.size()),
      locations_(in.passengers.size() + in.parcels.size()),
      removal_cache_(locations_.size()) {

    active_taxis_.reserve(in.taxis.size());
    const auto evaluation = solution.evaluate(in);

    benefit_ = evaluation.benefit;
    for (std::size_t k = 0; k < solution.routes.size(); ++k)
        if (!solution.routes[k].empty()) rebuild_route(k, solution.routes[k]);
}

void SearchState::copy_from(const SearchState& other) {
    if (this == &other) return;

    // Rebuild versions are globally unique; version 0 denotes a cleared route.
    // Equal versions on the same instance/taxi already contain identical data.
    for (std::size_t taxi = 0; taxi < routes_by_taxi_.size(); ++taxi)
        if (routes_by_taxi_[taxi].version() != other.routes_by_taxi_[taxi].version())
            routes_by_taxi_[taxi].copy_from(other.routes_by_taxi_[taxi]);
    active_taxis_.reserve(taxi_count());
    active_taxis_ = other.active_taxis_;
    locations_ = other.locations_;
    removal_cache_ = other.removal_cache_;
    empty_taxi_index_ = other.empty_taxi_index_;
    benefit_ = other.benefit_;
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
    return request < n ? instance_->passengers[request].revenue :
                         instance_->parcels[request - n].revenue;
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
    auto& slot = routes_by_taxi_[taxi];
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

bool SearchState::remove_requests(std::size_t taxi, const std::vector<std::size_t>& requests) {
    if (taxi >= taxi_count() || requests.empty()) return false;
    const auto& old = cache(taxi);
    std::vector<bool> excluded(old.nodes().size(), false);
    double lost_revenue = 0;
    for (const auto request : requests) {
        if (request >= request_count()) return false;
        const auto loc = location(request);
        if (loc.taxi != static_cast<int>(taxi) || excluded[loc.pickup]) return false;
        excluded[loc.pickup] = excluded[loc.dropoff] = true;
        lost_revenue += revenue(request);
    }
    RouteCandidate candidate(*instance_, taxi, old);
    auto& route = route_scratch_;
    route.clear();
    for (std::size_t first = 0; first < old.nodes().size();) {
        if (excluded[first]) { ++first; continue; }
        auto end = first + 1;
        while (end < old.nodes().size() && !excluded[end]) ++end;
        candidate.add_range(first, end);
        route.insert(route.end(), old.nodes().begin() + first, old.nodes().begin() + end);
        first = end;
    }
    const auto new_cost = candidate.cost_if_feasible();
    if (!new_cost) return false;
    benefit_ += old.cost() - *new_cost - lost_revenue;
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
    commit_validated_route(taxi, relocation_check_, old_cost - *cost);
    return true;
}

bool SearchState::replace_request(std::size_t outgoing, std::size_t incoming,
                                  std::size_t a, std::size_t b) {
    if (outgoing >= request_count() || incoming >= request_count() ||
        location(outgoing).taxi < 0 || location(incoming).taxi >= 0) return false;
    const auto loc = location(outgoing);
    const auto taxi = static_cast<std::size_t>(loc.taxi);
    const auto old_cost = cache(taxi).cost();
    auto& route = route_scratch_;
    route = cache(taxi).nodes();
    route.erase(route.begin() + loc.dropoff);
    route.erase(route.begin() + loc.pickup);
    const bool passenger = incoming < passenger_count();
    if (a > b || b > route.size() || (passenger && a != b)) return false;
    const auto boundary_allowed = [&](std::size_t position) {
        return !position || route[position - 1] >= taxi_count() + passenger_count();
    };
    if (!boundary_allowed(a) || !boundary_allowed(b)) return false;
    route.insert(route.begin() + b, dropoff_node(incoming));
    route.insert(route.begin() + a, pickup_node(incoming));
    relocation_check_.rebuild(*instance_, taxi, route);
    RouteCandidate candidate(*instance_, taxi, relocation_check_);
    candidate.add_range(0, route.size());
    const auto cost = candidate.cost_if_feasible();
    if (!cost) return false;
    const double gain = revenue(incoming) - revenue(outgoing) + old_cost - *cost;
    if (gain <= 1e-9) return false;
    commit_validated_route(taxi, relocation_check_, gain);
    return true;
}

void SearchState::commit_validated_route(std::size_t taxi, RouteCache& updated,
                                        double gain) {
    const auto k = taxi_count(), n = passenger_count(), m = instance_->parcels.size();
    const auto request_id = [&](std::size_t node) {
        return node < k + 2 * n ? (node - k) % n : n + (node - k - 2 * n) % m;
    };
    auto& slot = routes_by_taxi_[taxi];
    for (const auto node : slot.nodes()) locations_[request_id(node)] = {};
    const bool was_active = !slot.nodes().empty();
    const bool active = !updated.nodes().empty();
    if (was_active != active) {
        const auto where = std::lower_bound(active_taxis_.begin(), active_taxis_.end(), taxi);
        if (active) active_taxis_.insert(where, taxi);
        else active_taxis_.erase(where);
    }
    // Keep the validated version and reuse the old route's buffers next time.
    std::swap(slot, updated);
    if (!active) slot.clear();
    for (std::size_t p = 0; p < slot.nodes().size(); ++p) {
        const auto node = slot.nodes()[p], request = request_id(node);
        auto& loc = locations_[request];
        loc.taxi = static_cast<int>(taxi);
        if (node == pickup_node(request)) loc.pickup = p;
        else loc.dropoff = p;
    }
    benefit_ += gain;
}

bool SearchState::swap_requests(std::size_t first, std::size_t second) {
    if (first >= request_count() || second >= request_count()) return false;
    if (second < first) std::swap(first, second);
    const auto a = location(first), b = location(second);
    if (a.taxi < 0 || b.taxi < 0 || a.taxi == b.taxi) return false;
    const auto make_route = [&](std::size_t outgoing, std::size_t incoming) {
        const auto loc = location(outgoing);
        auto nodes = cache(loc.taxi).nodes();
        nodes.erase(nodes.begin() + loc.dropoff);
        nodes.erase(nodes.begin() + loc.pickup);
        const auto delivery_boundary = incoming < passenger_count() ? loc.pickup : loc.dropoff - 1;
        nodes.insert(nodes.begin() + delivery_boundary, dropoff_node(incoming));
        nodes.insert(nodes.begin() + loc.pickup, pickup_node(incoming));
        return nodes;
    };
    const auto nodes_a = make_route(first, second), nodes_b = make_route(second, first);
    RouteCache new_a(*instance_, a.taxi, nodes_a), new_b(*instance_, b.taxi, nodes_b);
    RouteCandidate checked_a(*instance_, a.taxi, new_a), checked_b(*instance_, b.taxi, new_b);
    checked_a.add_range(0, nodes_a.size());
    checked_b.add_range(0, nodes_b.size());
    const auto cost_a = checked_a.cost_if_feasible(), cost_b = checked_b.cost_if_feasible();
    if (!cost_a || !cost_b) return false;
    const double gain = cache(a.taxi).cost() + cache(b.taxi).cost() - *cost_a - *cost_b;
    if (gain <= 1e-9) return false;
    commit_route_pair(a.taxi, std::move(new_a), b.taxi, std::move(new_b), gain);
    return true;
}

bool SearchState::exchange_route_suffixes(std::size_t taxi_a, std::size_t cut_a,
                                         std::size_t taxi_b, std::size_t cut_b) {
    if (taxi_a == taxi_b || taxi_a >= taxi_count() || taxi_b >= taxi_count()) return false;
    const auto& old_a = cache(taxi_a);
    const auto& old_b = cache(taxi_b);
    if (cut_a > old_a.nodes().size() || cut_b > old_b.nodes().size()) return false;
    for (const auto& loc : locations_) {
        if ((loc.taxi == static_cast<int>(taxi_a) && loc.pickup < cut_a && loc.dropoff >= cut_a) ||
            (loc.taxi == static_cast<int>(taxi_b) && loc.pickup < cut_b && loc.dropoff >= cut_b))
            return false;
    }
    std::vector<std::size_t> nodes_a(old_a.nodes().begin(), old_a.nodes().begin() + cut_a);
    nodes_a.insert(nodes_a.end(), old_b.nodes().begin() + cut_b, old_b.nodes().end());
    std::vector<std::size_t> nodes_b(old_b.nodes().begin(), old_b.nodes().begin() + cut_b);
    nodes_b.insert(nodes_b.end(), old_a.nodes().begin() + cut_a, old_a.nodes().end());
    RouteCache new_a(*instance_, taxi_a, nodes_a), new_b(*instance_, taxi_b, nodes_b);
    RouteCandidate checked_a(*instance_, taxi_a, new_a), checked_b(*instance_, taxi_b, new_b);
    checked_a.add_range(0, nodes_a.size());
    checked_b.add_range(0, nodes_b.size());
    const auto cost_a = checked_a.cost_if_feasible(), cost_b = checked_b.cost_if_feasible();
    if (!cost_a || !cost_b) return false;
    const double gain = old_a.cost() + old_b.cost() - *cost_a - *cost_b;
    if (gain <= 1e-9) return false;

    commit_route_pair(taxi_a, std::move(new_a), taxi_b, std::move(new_b), gain);
    return true;
}

void SearchState::commit_route_pair(std::size_t taxi_a, RouteCache new_a,
                                   std::size_t taxi_b, RouteCache new_b, double gain) {
    // Clear both old assignments before assigning either new route: requests
    // transferred from B to A must not be cleared again while rebuilding B.
    for (auto& loc : locations_)
        if (loc.taxi == static_cast<int>(taxi_a) || loc.taxi == static_cast<int>(taxi_b)) loc = {};
    const auto assign = [&](std::size_t taxi, RouteCache updated) {
        auto where = std::lower_bound(active_taxis_.begin(), active_taxis_.end(), taxi);
        const bool active = where != active_taxis_.end() && *where == taxi;
        if (updated.nodes().empty()) {
            if (active) active_taxis_.erase(where);
            routes_by_taxi_[taxi].clear();
        } else {
            if (!active) active_taxis_.insert(where, taxi);
            routes_by_taxi_[taxi] = std::move(updated);
        }
        const auto k = instance_->taxis.size(), n = passenger_count(), m = instance_->parcels.size();
        const auto& nodes = cache(taxi).nodes();
        for (std::size_t p = 0; p < nodes.size(); ++p) {
            const auto node = nodes[p];
            const auto request = node < k + 2 * n ? (node - k) % n : n + (node - k - 2 * n) % m;
            auto& loc = locations_[request];
            loc.taxi = static_cast<int>(taxi);
            if (node == pickup_node(request)) loc.pickup = p;
            else loc.dropoff = p;
        }
    };
    assign(taxi_a, std::move(new_a));
    assign(taxi_b, std::move(new_b));
    benefit_ += gain;
}


} // namespace soict::alns

// END src/search/alns/SearchState.cpp

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
    // Spend the search budget on collecting diverse routes, then let the
    // final route-pool master combine them. The former 60,000-step schedule
    // can still be selected with --disable-path-generation and
    // --cooling 0.9998465061085267.
    bool local_search = true;
    bool clean_cut_2opt_star = true;
    bool route_pool_repair = true;
    bool path_generation = true;
    bool paths_only = false;
    std::size_t path_prefix_limit = 1000000;
    std::size_t route_pool_limit = 400;
    std::size_t path_pool_limit = 400;
    bool candidate_sampling = true;
    double enumeration_seconds = 2;
    double generation_seconds = 140;
    std::size_t fleet_refinement_interval = 2;
    std::size_t fleet_refinement_cycles = 3;
    bool light_fleet_refinement = true;
};
} // namespace soict::alns

// END include/search/alns/Config.hpp
#include <cstddef>

namespace soict::alns {
struct Result { Solution solution; };
Result solve(const Instance& instance, const Config& config);
} // namespace soict::alns

// END include/search/alns/ALNS.hpp

// BEGIN include/search/alns/operators/LocalSearch/LocalSearch.hpp

// BEGIN include/search/alns/operators/LocalSearch/Move.hpp

// BEGIN include/search/alns/operators/Destroy/DestroyOperator.hpp
#include <cstddef>
#include <random>
#include <string_view>
#include <vector>

namespace soict::alns {
class DestroyOperator {
public:
    virtual ~DestroyOperator() = default;
    virtual std::vector<std::size_t> apply(SearchState& state, std::size_t count,
                                           std::mt19937_64& rng) const = 0;
};
} // namespace soict::alns

// END include/search/alns/operators/Destroy/DestroyOperator.hpp

namespace soict::alns {
class Move {
public:
    virtual ~Move() = default;
    virtual bool improve(SearchState& state) const = 0;
};
} // namespace soict::alns

// END include/search/alns/operators/LocalSearch/Move.hpp
#include <chrono>
#include <memory>
#include <vector>

namespace soict::alns {
class LocalSearch {
public:
    void add_move(std::unique_ptr<Move> move) { moves_.push_back(std::move(move)); }
    bool search(SearchState& state, std::chrono::steady_clock::time_point deadline =
        std::chrono::steady_clock::time_point::max()) const;
private:
    std::vector<std::unique_ptr<Move>> moves_;
};
} // namespace soict::alns

// END include/search/alns/operators/LocalSearch/LocalSearch.hpp

// BEGIN include/search/alns/operators/LocalSearch/ParcelRelocate.hpp

// BEGIN include/search/alns/operators/LocalSearch/MoveCache.hpp

// BEGIN include/search/alns/operators/Repair/InsertionSearch.hpp
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
// Immutable request data reused by repeated AddRequest / ProfitInsertion cache
// queries. The instance must remain unchanged while its empty-taxi index lives.
struct RequestSegments {
    bool passenger;
    Segment pickup, delivery;
    double revenue, weight;
};
class EmptyTaxiIndex {
public:
    explicit EmptyTaxiIndex(const Instance& instance);
    // Extend an unfinished request's index on first use. A solver process
    // searches in one thread, so this shared cache has no locking overhead.
    bool ensure(std::size_t request, Deadline deadline = Deadline::max()) const;
    bool complete(std::size_t request) const {
        return complete_[request];
    }
    const std::vector<EmptyTaxiChoice>& choices(std::size_t request) const {
        return choices_[request];
    }
    const RequestSegments& request_segments(std::size_t request) const {
        return request_segments_[request];
    }

private:
    const Instance* instance_;
    mutable std::vector<std::vector<EmptyTaxiChoice>> choices_;
    mutable std::vector<std::size_t> next_taxi_;
    mutable std::vector<bool> complete_;
    // Allocated to the full request count at construction, never resized.
    std::vector<RequestSegments> request_segments_;
};
std::shared_ptr<const EmptyTaxiIndex> build_empty_taxi_index(
    const SearchState& empty_state, Deadline deadline);
// The supplied route may be a temporary route with another request removed.
// A non-positive insertion can still participate in an improving replacement.
std::optional<InsertionChoice> best_insertion_on_route(const SearchState& state,
    std::size_t request, std::size_t taxi, const RouteCache& route,
    double minimum_gain = -std::numeric_limits<double>::infinity());

// Reuse an unserved request's route choice while that route is unchanged.
// A scan made above a gain threshold is reused only when the current best
// meets that threshold; otherwise the route is rescanned exactly.


// Exact positive choices at a fixed floor, independent of other taxis' gains.
// The dense table and winners are allocated once and reused across repairs.
class PositiveInsertionCache {
public:
    // Refresh only the requested row; unchanged routes retain exact choices.
    const std::optional<InsertionChoice>& best(const SearchState&, std::size_t request);
    void prepare(const SearchState&, const std::vector<std::size_t>& requests);
    void refresh_taxi(const SearchState&, std::size_t taxi,
                      const std::vector<std::size_t>& requests);
    const std::optional<InsertionChoice>& best(std::size_t request) const {
        return entries_[requests_ * taxis_ + request].choice;
    }

private:
    struct Entry {
        std::uint64_t version = std::numeric_limits<std::uint64_t>::max();
        std::optional<InsertionChoice> choice;
    };
    std::shared_ptr<const EmptyTaxiIndex> index_;
    std::size_t taxis_ = 0, requests_ = 0;
    // First requests*taxis cells, followed by one winner cell per request.
    std::vector<Entry> entries_;
    void initialize(const SearchState&);
    void refresh_entry(const SearchState&, std::size_t request, std::size_t taxi);
    void select_winner(std::size_t request);
};

// Repair only inserts requests, so an accepted insertion changes one taxi.
// Retain the top two positions per request and active taxi; rescan that taxi
// after insertion while keeping the other taxis' positions unchanged.

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
    std::optional<RelocationDescriptor> best() const {
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
    bool valid() const { return served_; }
    bool removal_feasible() const { return removal_gain_.has_value(); }
    double removal_savings() const { return removal_savings_; }
    std::size_t length(std::size_t taxi) const;
    std::optional<double> gain(std::size_t taxi,
        std::size_t pickup_boundary, std::size_t delivery_boundary) const;
    std::optional<double> gain(std::size_t taxi, const RouteCache& route,
        std::size_t pickup_boundary, std::size_t delivery_boundary) const;
    // Benefit delta from changed travel-cost arcs; feasibility is checked by gain().
    std::optional<double> calculate_delta_fast(std::size_t taxi,
        const RouteCache& route, std::size_t pickup_boundary,
        std::size_t delivery_boundary) const;
    std::optional<FastRouteCheck> pickup_prefix(
        std::size_t taxi, const RouteCache& route, std::size_t pickup_boundary) const;
    std::optional<double> gain_from_prefix(std::size_t taxi,
        const RouteCache& route, std::size_t pickup_boundary,
        std::size_t delivery_boundary, const FastRouteCheck& prefix) const;
private:
    const SearchState& state_;
    std::size_t request_, source_, pickup_at_, delivery_at_;
    bool served_ = false;
    std::optional<double> removal_gain_;
    double removal_savings_ = 0;
    Segment pickup_, delivery_;
    std::size_t original_position(std::size_t position) const;
    bool source_boundary_allowed(const RouteCache& route,
                                                std::size_t boundary) const;
    void add_source_range(FastRouteCheck& candidate, std::size_t first,
                          std::size_t end) const;
};

class ParcelRelocate final : public Move {
public:
    bool improve(SearchState&) const override;
private:
    mutable MoveCache cache_;
    std::optional<InsertionChoice> best_on_taxi(
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
    bool valid() const { return served_; }
    bool removal_feasible() const { return removal_gain_.has_value(); }
    double removal_savings() const { return removal_savings_; }
    std::size_t length(std::size_t taxi) const;
    std::optional<double> gain(std::size_t taxi,
                                              std::size_t boundary) const;
    std::optional<double> gain(std::size_t taxi,
        const RouteCache& route, std::size_t boundary) const;
    // Benefit delta from changed travel-cost arcs; feasibility is checked by gain().
    std::optional<double> calculate_delta_fast(std::size_t taxi,
        const RouteCache& route, std::size_t boundary) const;
private:
    const SearchState& state_;
    std::size_t request_, source_, removed_at_;
    bool served_ = false;
    std::optional<double> removal_gain_;
    double removal_savings_ = 0;
    std::size_t original_position(std::size_t position) const;
    void add_source_range(FastRouteCheck& candidate, std::size_t first,
                          std::size_t end) const;
};

class PassengerRelocate final : public Move {
public:
    bool improve(SearchState&) const override;
private:
    mutable MoveCache cache_;
    std::optional<InsertionChoice> best_on_taxi(
        const SearchState&, const PassengerRelocationContext&, std::size_t request,
        std::size_t taxi, const RouteCache&, std::uint64_t source_version) const;
};
} // namespace soict::alns

// END include/search/alns/operators/LocalSearch/PassengerRelocate.hpp

// BEGIN include/search/alns/operators/LocalSearch/AddRequest.hpp

namespace soict::alns {
class AddRequest final : public Move {
public:
    bool improve(SearchState& state) const override;
private:
    mutable PositiveInsertionCache cache_;
    mutable std::vector<std::size_t> requests_;
};
} // namespace soict::alns

// END include/search/alns/operators/LocalSearch/AddRequest.hpp

// BEGIN include/search/alns/operators/LocalSearch/RemoveRequest.hpp

namespace soict::alns {
class RemoveRequest final : public Move {
public:
    bool improve(SearchState& state) const override;
};
} // namespace soict::alns

// END include/search/alns/operators/LocalSearch/RemoveRequest.hpp

// BEGIN include/search/alns/operators/LocalSearch/CleanCut2OptStar.hpp


namespace soict::alns {
// Exchange suffixes at boundaries where every request is wholly on one side.
class CleanCut2OptStar final : public Move {
public:
    bool improve(SearchState& state) const override;
private:
};
} // namespace soict::alns

// END include/search/alns/operators/LocalSearch/CleanCut2OptStar.hpp

// BEGIN include/search/alns/operators/LocalSearch/ReplaceRequest.hpp
#include <queue>

namespace soict::alns {
struct ReplacementChoice {
    std::size_t outgoing;
    InsertionChoice insertion;
    double gain;
};

class ReplaceRequest final : public Move {
public:
    // The original scan remains available for differential validation.
    ReplaceRequest() = default;
    bool improve(SearchState&) const override;
    std::optional<ReplacementChoice> best_choice(const SearchState&) const;
private:

    struct Row {
        std::uint64_t reduced_version = 0;
        RouteCache reduced;
        double removal_gain = 0;
    };
    mutable std::vector<Row> rows_;

    struct Descriptor {
        std::uint64_t generation = 0;
        std::optional<ReplacementChoice> choice;
    };
    struct QueuedChoice {
        ReplacementChoice choice;
        std::uint64_t route_version, generation;
    };
    struct LowerPriority {
        bool operator()(const QueuedChoice&, const QueuedChoice&) const;
    };
    using Queue = std::priority_queue<QueuedChoice, std::vector<QueuedChoice>, LowerPriority>;
    mutable const Instance* smd_instance_ = nullptr;
    mutable std::vector<std::uint64_t> route_versions_;
    mutable std::vector<bool> unserved_;
    mutable std::vector<std::vector<Descriptor>> descriptors_; // [taxi][incoming]
    mutable Queue queue_;
    void refresh_descriptors(const SearchState&, std::size_t taxi,
        const std::vector<std::size_t>& outgoing,
        const std::vector<std::size_t>& incoming) const;
    void synchronize(const SearchState&) const;
    void discard_stale() const;
    void enqueue(std::size_t taxi, std::size_t incoming) const;
    void compact_queue() const;
};
} // namespace soict::alns

// END include/search/alns/operators/LocalSearch/ReplaceRequest.hpp

// BEGIN include/search/alns/operators/LocalSearch/SwapRequest.hpp

#include <optional>
#include <queue>

namespace soict::alns {
struct RequestSwapChoice {
    std::size_t first, second;
    double gain;
};

// Both endpoints move together; incoming passenger delivery follows pickup.
// Returns no value for an invalid pair or infeasible completed routes.
std::optional<double> request_swap_gain(const SearchState&, std::size_t first,
                                       std::size_t second);

class SwapRequest final : public Move {
public:
    SwapRequest() = default;
    bool improve(SearchState&) const override;
    std::optional<RequestSwapChoice> best_choice(const SearchState&) const;
private:
    struct Descriptor {
        std::uint64_t generation = 0;
        std::optional<RequestSwapChoice> choice;
    };
    struct QueuedChoice {
        RequestSwapChoice choice;
        std::size_t taxi_a, taxi_b;
        std::uint64_t version_a, version_b, generation;
    };
    struct LowerPriority {
        bool operator()(const QueuedChoice&, const QueuedChoice&) const;
    };
    using Queue = std::priority_queue<QueuedChoice, std::vector<QueuedChoice>, LowerPriority>;
    mutable const Instance* instance_ = nullptr;
    mutable std::vector<std::uint64_t> versions_;
    mutable std::vector<std::vector<Descriptor>> descriptors_; // taxi_a < taxi_b
    mutable Queue queue_;
    void synchronize(const SearchState&) const;
    void enqueue(std::size_t taxi_a, std::size_t taxi_b) const;
};
} // namespace soict::alns

// END include/search/alns/operators/LocalSearch/SwapRequest.hpp

// BEGIN include/search/alns/operators/Repair/RoutePoolRepair.hpp
#include <map>
#include <iosfwd>

namespace soict::alns {
// A restricted fleet repair over complete, independently feasible taxi routes.
// Identical served sets on one taxi retain only the most profitable sequence.
class RoutePoolRepair {
public:
    explicit RoutePoolRepair(const Instance& instance, std::size_t limit = 200, std::uint64_t seed = 1);
    void remember(const SearchState& state);
    // Enumerate feasible prefixes independently for each taxi. Only completed
    // positive-profit routes enter the existing capped, served-set bank.
    // Completion refers to prefix enumeration, not retention of every column.
    void generate(Deadline deadline, std::size_t prefix_limit = 50000, bool priced = false);
    // Perturb complete routes and repair with cached insertion. Preserve all
    // sampled alternatives and return the best feasible sampled fleet.
    Solution generate_fleets(Deadline deadline, std::shared_ptr<const EmptyTaxiIndex> index = {},
                             const Solution* initial = nullptr, bool refine = true,
                             bool clean_cut = true, std::size_t refinement_interval = 2,
                             bool light_refinement = true, std::size_t refinement_cycles = 3);
    void generate_replacements(Deadline deadline, const SearchState& fleet);
    void set_limit(std::size_t limit);
    bool enumeration_complete() const { return completed_taxis_ == pool_.size(); }
    bool improve(SearchState& state, Deadline deadline);
    // Statistics and completion describe the most recent improve() call,
    // before any subsequent generation changes that call's route bank.
private:
    struct Column {
        std::vector<std::size_t> requests, route;
        double profit = 0;
    };
    struct Arc { std::size_t request; double latest_departure; };
    const Instance& instance_;
    std::size_t limit_, column_count_ = 0, nodes_ = 0;
    bool timed_out_ = false;
    std::size_t completed_taxis_ = 0;
    std::vector<double> floors_;
    std::uint64_t seed_ = 1;
    std::uint64_t fleet_calls_ = 0;
    std::uint64_t pricing_calls_ = 0;
    std::uint64_t generation_calls_ = 0;
    std::vector<std::map<std::vector<std::size_t>, Column>> pool_;
    std::vector<std::uint64_t> versions_;
    std::vector<std::vector<Arc>> next_;
    std::vector<double> delivery_lower_;
    bool generation_index_ready_ = false;
    std::uint64_t bank_revision_ = 0;
    mutable std::uint64_t prices_revision_ = std::numeric_limits<std::uint64_t>::max();
    mutable std::vector<double> cached_prices_;
    void add(const SearchState&, std::size_t taxi, bool enforce_limit);
    void add_column(std::size_t taxi, const std::vector<std::size_t>& requests,
                    const std::vector<std::size_t>& route, double profit, bool enforce_limit);
    std::vector<double> estimate_prices(Deadline deadline = Deadline::min()) const;
};
} // namespace soict::alns

// END include/search/alns/operators/Repair/RoutePoolRepair.hpp

#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <numeric>
#include <random>
#include <sstream>
#include <iomanip>
#include <stdexcept>

namespace soict::alns {
Result solve(const Instance& instance, const Config& config) {
    using Clock = std::chrono::steady_clock;


    const auto start = Clock::now();
    const auto deadline = start + std::chrono::duration_cast<Clock::duration>(
                                  std::chrono::duration<double>(config.time_limit));
    const double polish_reserve = config.route_pool_repair ? 1.0 : 0.0;
    const auto search_deadline = deadline - std::chrono::duration_cast<Clock::duration>(
        std::chrono::duration<double>(std::min(polish_reserve, config.time_limit * 0.05)));
    LocalSearch local_search;
    local_search.add_move(std::make_unique<RemoveRequest>());
    local_search.add_move(std::make_unique<AddRequest>());
    local_search.add_move(std::make_unique<PassengerRelocate>());
    local_search.add_move(std::make_unique<ParcelRelocate>());
    if (config.clean_cut_2opt_star) {
        auto move = std::make_unique<CleanCut2OptStar>();
        local_search.add_move(std::move(move));
    }
    local_search.add_move(std::make_unique<SwapRequest>());
    local_search.add_move(std::make_unique<ReplaceRequest>());

    Solution empty;
    empty.routes.resize(instance.taxis.size());
    SearchState current(instance, std::move(empty));
    const auto index_deadline = std::min(deadline, Clock::now() +
        std::chrono::duration_cast<Clock::duration>(
            std::chrono::duration<double>(std::min(1.0, config.time_limit * 0.05))));
    current.set_empty_taxi_index(build_empty_taxi_index(current, index_deadline));
    std::unique_ptr<RoutePoolRepair> route_pool;
    bool paths_only = config.paths_only;
    if (config.route_pool_repair) {
        route_pool = std::make_unique<RoutePoolRepair>(instance, config.route_pool_limit,config.seed);
        if (config.path_generation || config.paths_only) {
            const auto generation_start = Clock::now();
            const auto generation_deadline = std::min(search_deadline, generation_start +
                std::chrono::duration_cast<Clock::duration>(std::chrono::duration<double>(
                    std::min(config.enumeration_seconds, config.time_limit * 0.2))));
            route_pool->generate(generation_deadline, config.path_prefix_limit);
            const bool initial_enumeration_complete=route_pool->enumeration_complete();
            const bool sampled = config.candidate_sampling &&
                (!initial_enumeration_complete || instance.taxis.size()>48);
            if (sampled) {
                const auto beam_start = Clock::now();
                // Leave a fixed selection budget instead of abandoning a
                // large fraction of short competition runs after an easy master.
                const auto selection_reserve=std::chrono::duration_cast<Clock::duration>(
                    std::chrono::duration<double>(std::min(3.0,config.time_limit*0.05)));
                const auto beam_deadline = std::min(search_deadline-selection_reserve,beam_start+
                    std::chrono::duration_cast<Clock::duration>(std::chrono::duration<double>(
                        config.generation_seconds)));
                std::size_t phase=0;
                do {
                    const auto sample_deadline = instance.taxis.size()>48 ?
                        std::min(beam_deadline,Clock::now()+std::chrono::seconds(6)) : beam_deadline;
                    const auto initial=current.solution();
                    SearchState generated(instance,route_pool->generate_fleets(sample_deadline,current.empty_taxi_index(),&initial,
                                          config.local_search,config.clean_cut_2opt_star,config.fleet_refinement_interval,
                                          config.light_fleet_refinement,config.fleet_refinement_cycles),
                                          current.insertion_scratch_handle());
                    generated.set_empty_taxi_index(current.empty_taxi_index());current.copy_from(generated);
                    if (instance.taxis.size()>48 && Clock::now()+std::chrono::seconds(2)<beam_deadline) {
                        const auto mix_deadline=std::min(beam_deadline,Clock::now()+std::chrono::seconds(6));
                        route_pool->improve(current,mix_deadline);
                        ++phase; (void)0;
                        if (Clock::now()+std::chrono::seconds(2)<beam_deadline) {
                            const auto price_deadline=std::min(beam_deadline,Clock::now()+std::chrono::seconds(3));
                            if (phase%2) route_pool->generate_replacements(price_deadline,current);
                            else route_pool->generate(price_deadline,config.path_prefix_limit,true);
                            (void)0;
                            if (!(phase%2)) (void)0;
                            (void)0;
                        }
                    }
                } while (instance.taxis.size()>48 && Clock::now()<beam_deadline);
                paths_only = true;
            }
            if (initial_enumeration_complete) {
                paths_only = true;
                if (!sampled) route_pool->set_limit(std::min(config.route_pool_limit, config.path_pool_limit));
            }
            (void)0;
        }
    }
    SearchState best = current;
    if (route_pool) route_pool->remember(current);
    if (route_pool) {
        route_pool->improve(best, deadline);
        (void)0;
        // Recombination can expose insertions and exchanges absent from the
        // retained bank. Polish only complete moves within the remaining budget.
        if (paths_only && config.local_search && Clock::now()+std::chrono::milliseconds(500)<deadline) {
            const auto before_polish=best.benefit();
            local_search.search(best,deadline);
            if (best.benefit()>before_polish+1e-9) route_pool->remember(best);
            (void)0;
        }
    }
    Result result;
    result.solution = best.solution();
    return result;
}
} // namespace soict::alns

// END src/search/alns/ALNS.cpp

// BEGIN src/search/alns/operators/Destroy/RandomRemoval.cpp

// BEGIN include/search/alns/operators/Destroy/RandomRemoval.hpp

namespace soict::alns {
class RandomRemoval final : public DestroyOperator {
public:
    std::vector<std::size_t> apply(SearchState&, std::size_t, std::mt19937_64&) const override;
};
} // namespace soict::alns

// END include/search/alns/operators/Destroy/RandomRemoval.hpp

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

// BEGIN include/search/alns/operators/Destroy/WorstRemoval.hpp

namespace soict::alns {
class WorstRemoval final : public DestroyOperator {
public:
    std::vector<std::size_t> apply(SearchState&, std::size_t, std::mt19937_64&) const override;
};
} // namespace soict::alns

// END include/search/alns/operators/Destroy/WorstRemoval.hpp

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

// BEGIN include/search/alns/operators/Destroy/RelatedRemoval.hpp

namespace soict::alns {
class RelatedRemoval final : public DestroyOperator {
public:
    std::vector<std::size_t> apply(SearchState&, std::size_t, std::mt19937_64&) const override;
};
} // namespace soict::alns

// END include/search/alns/operators/Destroy/RelatedRemoval.hpp

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
#include <numeric>

namespace soict::alns {
namespace {
RequestSegments make_request_segments(const Instance& in, std::size_t request) {
    const auto n = in.passengers.size();
    const bool passenger = request < n;
    const auto pickup = passenger ? in.passenger_pickup(request) :
                                   in.parcel_pickup(request - n);
    const auto delivery = passenger ? in.passenger_dropoff(request) :
                                     in.parcel_dropoff(request - n);
    return {passenger, single_node(in, pickup), single_node(in, delivery),
            passenger ? in.passengers[request].revenue : in.parcels[request - n].revenue,
            passenger ? 0 : in.parcels[request - n].weight};
}
}

EmptyTaxiIndex::EmptyTaxiIndex(const Instance& instance)
    : instance_(&instance),
      choices_(instance.passengers.size() + instance.parcels.size()),
      next_taxi_(choices_.size(), 0), complete_(choices_.size(), false),
      request_segments_(choices_.size()) {
    for (std::size_t request = 0; request < request_segments_.size(); ++request)
        request_segments_[request] = make_request_segments(instance, request);
}

bool EmptyTaxiIndex::ensure(std::size_t request, Deadline deadline) const {
    if (complete_[request]) return true;
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
                            InsertionScratch& scratch,
                            double global_second = -std::numeric_limits<double>::infinity(),
                            bool best_only = false) {
    InsertionOptions result;
    const auto& in = state.instance();
    if (data.weight > in.taxis[taxi].parcel_capacity + 1e-8) return result;
    const auto length = route.nodes().size();
    const auto pickup = data.pickup.first, delivery = data.delivery.first;
    const auto& nodes = route.nodes();
    // The objective change involves only replaced arcs, even for directed
    // nonmetric costs. Use it to reject candidates before segment joins.
    const auto buffers = scratch.buffers(length);
    auto* pickup_delta = buffers.pickup;
    auto* delivery_delta = buffers.delivery;
    auto* adjacent_delta = buffers.adjacent;
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
    auto* suffix_delivery = buffers.suffix;
    if (!data.passenger) {
        // The backward pass overwrites every other used suffix element.
        suffix_delivery[length + 1] = std::numeric_limits<double>::infinity();
        for (std::size_t pos = length + 1; pos-- > 0;)
            suffix_delivery[pos] = std::min(delivery_delta[pos], suffix_delivery[pos + 1]);
    }
    for (std::size_t a = 0; a <= length; ++a) {
        if (!route.boundary_allowed(in, a)) continue;
        const auto threshold = [&] { return std::max(global_second,
            best_only && result.best ? result.best->gain : result.second_gain); };
        const auto best_delta = data.passenger ? adjacent_delta[a] :
            std::min(adjacent_delta[a], pickup_delta[a] + suffix_delivery[a + 1]);
        if (data.revenue - best_delta + 1e-8 < threshold()) continue;
        FastRouteCheck prefix(in, taxi, route);
        prefix.range(0, a);
        prefix.segment(data.pickup);
        // The pickup prefix is unchanged for every delivery position below.
        if (!prefix.feasible()) continue;
        if (data.passenger) {
            auto candidate = prefix;
            candidate.segment(data.delivery);
            candidate.range(a, length);
            if (candidate.feasible())
                consider(result, {request, taxi, a, a,
                    data.revenue - adjacent_delta[a]});
        } else {
            const auto evaluate = [&](std::size_t b, double delta) {
                auto candidate = prefix;
                candidate.range(a, b);
                candidate.segment(data.delivery);
                candidate.range(b, length);
                if (candidate.feasible())
                    consider(result, {request, taxi, a, b, data.revenue - delta});
            };
            for (std::size_t b = a; b <= length; ++b) {
                if (!route.boundary_allowed(in, b)) continue;
                const auto delta = b == a ? adjacent_delta[a] :
                                   pickup_delta[a] + delivery_delta[b];
                if (data.revenue - delta + 1e-8 < threshold()) continue;
                evaluate(b, delta);
            }
        }
    }
    return result;
}
} // namespace

std::optional<InsertionChoice> best_insertion_on_route(const SearchState& state,
    std::size_t request, std::size_t taxi, const RouteCache& route, double minimum_gain) {
    if (request >= state.request_count() || taxi >= state.taxi_count() ||
        state.location(request).taxi >= 0) return std::nullopt;
    return scan_route(state, request, taxi, route, request_segments(state, request),
                      state.insertion_scratch(), minimum_gain, true).best;
}

void PositiveInsertionCache::refresh_entry(const SearchState& state,
    std::size_t request, std::size_t taxi) {
    auto& entry = entries_[request * taxis_ + taxi];
    const auto& route = state.cache(taxi);
    entry.choice.reset();
    if (state.is_active(taxi)) {
        entry.choice = scan_route(state, request, taxi, route,
            state.empty_taxi_index()->request_segments(request),
            state.insertion_scratch(), 1e-9, true).best;
        if (entry.choice && entry.choice->gain <= 1e-9) entry.choice.reset();
    } else {
        for (const auto& empty : state.empty_taxi_index()->choices(request))
            if (empty.taxi == taxi) {
                if (empty.gain > 1e-9)
                    entry.choice = InsertionChoice{request, taxi, 0, 0, empty.gain};
                break;
            }
    }
    entry.version = route.version();
}

void PositiveInsertionCache::select_winner(std::size_t request) {
    auto& winner = entries_[requests_ * taxis_ + request].choice;
    winner.reset();
    for (std::size_t taxi = 0; taxi < taxis_; ++taxi) {
        const auto& choice = entries_[request * taxis_ + taxi].choice;
        if (choice && (!winner || choice->gain > winner->gain)) winner = choice;
    }
}

void PositiveInsertionCache::initialize(const SearchState& state) {
    const auto& index = state.empty_taxi_index();

    if (index_ != index || taxis_ != state.taxi_count() ||
        requests_ != state.request_count()) {
        index_ = index;
        taxis_ = state.taxi_count();
        requests_ = state.request_count();

        entries_.assign(requests_ * (taxis_ + 1), Entry{});
    }
}

const std::optional<InsertionChoice>& PositiveInsertionCache::best(const SearchState& state,
    std::size_t request) {
    initialize(state);
    if (state.location(request).taxi >= 0) {
        entries_[requests_ * taxis_ + request].choice.reset();
        return best(request);
    }
    index_->ensure(request);
    for (std::size_t taxi = 0; taxi < taxis_; ++taxi)
        if (entries_[request * taxis_ + taxi].version != state.cache(taxi).version()) {
            refresh_entry(state, request, taxi);
        }
    select_winner(request);
    return best(request);
}

void PositiveInsertionCache::prepare(const SearchState& state,
    const std::vector<std::size_t>& requests) {
    initialize(state);
    const auto& index = index_;
    for (const auto request : requests) {
        if (state.location(request).taxi >= 0) continue;
        index->ensure(request);
        for (std::size_t taxi = 0; taxi < taxis_; ++taxi)
            if (entries_[request * taxis_ + taxi].version != state.cache(taxi).version())
                refresh_entry(state, request, taxi);
        select_winner(request);
    }
}

void PositiveInsertionCache::refresh_taxi(const SearchState& state,
    std::size_t taxi, const std::vector<std::size_t>& requests) {
    for (const auto request : requests) {
        if (state.location(request).taxi >= 0) continue;
        const auto previous = best(request);
        refresh_entry(state, request, taxi);
        const auto& updated = entries_[request * taxis_ + taxi].choice;
        if (previous && previous->taxi == taxi) select_winner(request);
        else if (updated && (!previous || updated->gain > previous->gain ||
                            (updated->gain == previous->gain && taxi < previous->taxi)))
            entries_[requests_ * taxis_ + request].choice = updated;
    }
}

} // namespace soict::alns

// END src/search/alns/operators/Repair/InsertionSearch.cpp

// BEGIN src/search/alns/operators/Repair/RoutePoolRepair.cpp

// BEGIN include/search/alns/operators/Repair/RoutePoolMaster.hpp
#include <vector>

namespace soict::alns {
struct MasterColumn {
    std::size_t taxi;
    std::vector<std::size_t> requests;
    double profit;
};
struct MasterResult {
    std::vector<int> selected;
    // Optional full-bank request prices for the next generation phase.
    // These guide route generation; they do not certify a restricted solve.
    std::vector<double> request_prices;
    double profit = 0, root_upper = 0;
    std::size_t nodes = 0, pivots = 0;
    bool timed_out = false;
    bool complete = false;
};
std::vector<double> route_master_prices(const std::vector<MasterColumn>& columns,
    std::size_t taxis, std::size_t requests, Deadline deadline);
bool route_master_uses_cplex();
// Restricted set-packing master. The input incumbent must be feasible.
MasterResult solve_route_master(const std::vector<MasterColumn>& columns,
    std::size_t taxis, std::size_t requests, const std::vector<int>& incumbent,
    Deadline deadline, bool verbose = true);
// Coordinated LP neighborhoods preserve a feasible fleet throughout. This
// heuristic does not claim that the unrestricted bank master is complete.
MasterResult improve_route_master(const std::vector<MasterColumn>& columns,
    std::size_t taxis, std::size_t requests, const std::vector<int>& incumbent,
    Deadline deadline, std::uint64_t seed = 1, const std::vector<double>& request_prices = {});
}

// END include/search/alns/operators/Repair/RoutePoolMaster.hpp
#include <algorithm>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <numeric>
#include <stdexcept>
#include <random>

namespace soict::alns {
RoutePoolRepair::RoutePoolRepair(const Instance& instance, std::size_t limit, std::uint64_t seed)
    : instance_(instance), limit_(limit), floors_(instance.taxis.size(),-std::numeric_limits<double>::infinity()), seed_(seed),
      pool_(instance.taxis.size()), versions_(pool_.size(), 0) {

}

void RoutePoolRepair::add(const SearchState& state, std::size_t taxi, bool enforce_limit) {
    const auto& nodes = state.cache(taxi).nodes();
    if (nodes.empty()) return;
    const auto k = state.taxi_count(), n = state.passenger_count();
    std::vector<std::size_t> requests;
    double revenue = 0;
    for (const auto node : nodes) {
        std::size_t request;
        if (node < k + n) request = node - k;
        else if (node >= k + 2*n && node < k + 2*n + instance_.parcels.size())
            request = n + node - k - 2*n;
        else continue;
        requests.push_back(request);
        revenue += state.revenue(request);
    }
    std::sort(requests.begin(), requests.end());
    const auto profit = revenue - state.cache(taxi).cost();
    add_column(taxi, requests, nodes, profit, enforce_limit);
}

void RoutePoolRepair::add_column(std::size_t taxi, const std::vector<std::size_t>& requests,
        const std::vector<std::size_t>& nodes, double profit, bool enforce_limit) {
    if (!std::isfinite(profit) || profit <= 1e-9) return;
    auto& bank = pool_[taxi];
    if (enforce_limit && bank.size() >= limit_ && profit < floors_[taxi]) return;
    const auto found = bank.find(requests);
    if (found != bank.end() && found->second.profit >= profit) return;
    bank[requests] = Column{requests, nodes, profit};
    ++bank_revision_;
    if (!enforce_limit) floors_[taxi] = -std::numeric_limits<double>::infinity();
    if (enforce_limit && bank.size() > limit_) {
        const auto worst = std::min_element(bank.begin(), bank.end(), [](const auto& a, const auto& b) {
            return a.second.profit < b.second.profit;
        });
        bank.erase(worst);
        floors_[taxi] = std::min_element(bank.begin(),bank.end(),[](const auto& a,const auto& b) {
            return a.second.profit < b.second.profit;
        })->second.profit;
    }
}

void RoutePoolRepair::generate(Deadline deadline, std::size_t prefix_limit, bool priced) {

    completed_taxis_ = 0;
    if (std::chrono::steady_clock::now() >= deadline) return;
    const auto& in = instance_;
    const auto n = in.passengers.size(), nr = n + in.parcels.size(), nv = in.node_count();
    auto prices=priced?estimate_prices(deadline):std::vector<double>(nr,0);
    const double scales[]={1.0,0.8,1.2};
    if (priced) {
        const auto scale=scales[generation_calls_%3];for (auto& price:prices) price*=scale;
        (void)0;
    }
    const auto generation_seed=seed_+0x9e3779b97f4a7c15ULL*generation_calls_++;
    struct Request {
        std::size_t pickup, delivery;
        double ready, latest, drop_ready, drop_latest, service, revenue, weight;
    };
    std::vector<Request> requests;
    for (std::size_t r = 0; r < nr; ++r) {
        if (r < n) {
            const auto& p = in.passengers[r];
            requests.push_back({in.passenger_pickup(r), in.passenger_dropoff(r),
                p.pickup.earliest, p.pickup.latest, 0, std::numeric_limits<double>::infinity(),
                p.service, p.revenue-prices[r], 0});
        } else {
            const auto& p = in.parcels[r-n];
            requests.push_back({in.parcel_pickup(r-n), in.parcel_dropoff(r-n),
                p.pickup.earliest, p.pickup.latest, p.delivery.earliest, p.delivery.latest,
                p.service, p.revenue-prices[r], p.weight});
        }
    }
    const auto m=in.parcels.size();
    if (!generation_index_ready_) {
    // Unrestricted shortest paths give optimistic travel times even on
    // directed/nonmetric matrices. Landmark triangle differences therefore
    // bound every legal service path without assuming direct arcs are metric.
    const auto preprocess_start=std::chrono::steady_clock::now();
    const auto preprocess_seconds=std::min(0.6,0.25*std::chrono::duration<double>(deadline-preprocess_start).count());
    const auto preprocess_deadline=preprocess_start+std::chrono::duration_cast<Deadline::duration>(
        std::chrono::duration<double>(std::max(0.0,preprocess_seconds)));
    std::vector<std::vector<double>> landmarks;
    std::vector<double> spread(nv,std::numeric_limits<double>::infinity());
    std::vector<unsigned char> used_landmark(nv,0);
    std::size_t landmark=0;
    for (std::size_t l=0;l<std::min<std::size_t>(8,nv);++l) {
        if (std::chrono::steady_clock::now()>=preprocess_deadline) break;
        std::vector<double> distance(in.travel_time.begin()+landmark*nv,in.travel_time.begin()+(landmark+1)*nv);
        distance[landmark]=0;std::vector<unsigned char> visited(nv,0);bool complete=true;
        for (std::size_t step=0;step<nv;++step) {
            if ((step&63)==0 && std::chrono::steady_clock::now()>=preprocess_deadline) {complete=false;break;}
            std::size_t v=nv;double value=std::numeric_limits<double>::infinity();
            for (std::size_t u=0;u<nv;++u) if (!visited[u] && distance[u]<value) {v=u;value=distance[u];}
            if (v==nv) break;
            visited[v]=1;
            for (std::size_t u=0;u<nv;++u) if (!visited[u])
                distance[u]=std::min(distance[u],value+in.travel_time[v*nv+u]);
        }
        if (!complete) break;
        used_landmark[landmark]=1;
        for (std::size_t u=0;u<nv;++u) spread[u]=std::min(spread[u],distance[u]);
        landmarks.push_back(std::move(distance));double farthest=-1;
        for (std::size_t u=0;u<nv;++u) if (!used_landmark[u] && spread[u]>farthest) {farthest=spread[u];landmark=u;}
    }
    std::vector<double> delivery_lower(nv*m,0);
    for (std::size_t node=0;node<nv;++node) for (std::size_t r=0;r<m;++r) {
        auto& lower=delivery_lower[node*m+r];const auto drop=in.parcel_dropoff(r);
        for (const auto& d:landmarks) {
            const double guard=1e-8+64*std::numeric_limits<double>::epsilon()*nv*std::max({1.0,d[drop],d[node]});
            lower=std::max(lower,d[drop]-d[node]-guard);
        }
    }
    (void)0;
    std::vector<std::vector<Arc>> next(nv);
    for (std::size_t node = 0; node < nv; ++node) {
        if (std::chrono::steady_clock::now() >= deadline) return;
        auto& arcs = next[node];
        for (std::size_t r = 0; r < nr; ++r) {
            const auto travel = in.travel_time[node*nv+requests[r].pickup];
            const auto guard = 8*std::numeric_limits<double>::epsilon()*
                std::max({1.0,std::abs(requests[r].latest),std::abs(travel)});
            const auto latest = requests[r].latest - travel + 1e-8 + guard;
            if (latest >= -1e-8) arcs.push_back({r, latest});
        }
        std::sort(arcs.begin(), arcs.end(), [](const auto& a, const auto& b) {
            return a.latest_departure > b.latest_departure ||
                (a.latest_departure == b.latest_departure && a.request < b.request);
        });
    }
    next_=std::move(next);delivery_lower_=std::move(delivery_lower);generation_index_ready_=true;
    (void)0;
    }
    const auto& next=next_;const auto& delivery_lower=delivery_lower_;
    completed_taxis_ = 0;
    std::vector<unsigned char> picked(nr, 0), opened(nr, 0);
    std::vector<std::size_t> route, served;
    route.reserve(2*nr); served.reserve(nr);
    for (std::size_t taxi = 0; taxi < pool_.size(); ++taxi) {
        if (std::chrono::steady_clock::now() >= deadline) break;
        // Fairly divide a finite generation budget across the remaining taxis.
        const auto now = std::chrono::steady_clock::now();
        const auto taxi_deadline = deadline == Deadline::max() ? deadline :
            now + (deadline-now)/static_cast<Deadline::duration::rep>(pool_.size()-taxi);
        std::size_t prefixes = 0;
        std::map<std::vector<std::size_t>,std::pair<double,Column>> priced_bank;
        double priced_floor=-std::numeric_limits<double>::infinity();
        std::size_t prefix_ceiling = prefix_limit;
        auto branch_deadline=taxi_deadline;
        bool exhausted = false, incomplete = false;
        std::size_t first_remaining=0;
        for (const auto& arc:next[taxi]) {
            const auto& q=requests[arc.request];
            if (q.weight<=in.taxis[taxi].parcel_capacity+1e-8 &&
                in.travel_time[taxi*nv+q.pickup]<=q.latest+1e-8) ++first_remaining;
        }
        const auto visit = [&](auto&& self, std::size_t last, double time, double cost,
                               double revenue, double load, std::size_t open_count) -> void {
            if (exhausted) return;
            if (prefixes >= prefix_ceiling || ((prefixes & 63) == 0 &&
                std::chrono::steady_clock::now() >= branch_deadline)) {
                exhausted = true; return;
            }
            ++prefixes;
            for (auto r:served) if (opened[r] && time+delivery_lower[last*m+r-n]>requests[r].drop_latest+1e-8)
                return;
            if (!open_count && revenue-cost > 1e-9 &&
                (priced ? (priced_bank.size()<limit_ || revenue-cost>=priced_floor) :
                    (pool_[taxi].size()<limit_ || revenue-cost>=floors_[taxi]))) {
                auto key = served;
                std::sort(key.begin(), key.end());
                if (!priced) add_column(taxi, key, route, revenue-cost, true);
                else {
                    const auto score=revenue-cost;
                    auto found=priced_bank.find(key);
                    if (found==priced_bank.end() || found->second.first<score) {
                        double profit=-cost;for (auto r:key)
                            profit+=r<n?in.passengers[r].revenue:in.parcels[r-n].revenue;
                        priced_bank[key]={score,Column{key,route,profit}};
                        if (priced_bank.size()>limit_) {
                            const auto worse=[](const auto& a,const auto& b){return a.second.first<b.second.first;};
                            priced_bank.erase(std::min_element(priced_bank.begin(),priced_bank.end(),worse));
                            priced_floor=std::min_element(priced_bank.begin(),priced_bank.end(),worse)->second.first;
                        }
                    }
                }
            }
            // An overflowing departure can still end a complete route: the
            // full evaluator checks arrivals, with no return leg. It cannot
            // be extended to another finite arrival.
            if (!std::isfinite(time)) return;
            auto count=next[last].size();
            std::uint64_t hash=0;
            if (priced) {
                const auto eligible=std::partition_point(next[last].begin(),next[last].end(),
                    [&](const auto& arc){return time<=arc.latest_departure;});
                count=static_cast<std::size_t>(eligible-next[last].begin());
                hash=generation_seed+taxi*0x94d049bb133111ebULL+last*0xbf58476d1ce4e5b9ULL;
                hash=(hash^(hash>>30))*0xbf58476d1ce4e5b9ULL;hash^=hash>>27;
            }
            const auto delivery_events=served.size(),events=delivery_events+count;
            const auto offset=events && (hash&1)?static_cast<std::size_t>(hash%events):0;
            for (std::size_t i=0;i<events;++i) {
                const auto event=offset?(i+offset)%events:i;
                if (event<delivery_events) {
                const auto r = served[event];
                if (!opened[r]) continue;
                const auto& q = requests[r];
                const double arrival = time + in.travel_time[last*nv+q.delivery];
                if (!std::isfinite(arrival) || arrival > q.drop_latest + 1e-8) continue;
                opened[r] = 0; route.push_back(q.delivery);
                self(self, q.delivery, std::max(arrival,q.drop_ready)+q.service,
                     cost+in.travel_cost[last*nv+q.delivery], revenue, load-q.weight, open_count-1);
                route.pop_back(); opened[r] = 1;
                if (exhausted) return;
                continue;
                }
                const auto& arc=next[last][event-delivery_events];
                if (!priced && time>arc.latest_departure) break;
                const auto r = arc.request;
                if (picked[r]) continue;
                const auto& q = requests[r];
                if (load+q.weight > in.taxis[taxi].parcel_capacity+1e-8) continue;
                const double arrival = time+in.travel_time[last*nv+q.pickup];
                // Test the actual arrival as well: cancellation in the sorted
                // departure threshold is not a substitute for feasibility.
                if (arrival > q.latest+1e-8) continue;
                const bool first=priced && served.empty();
                if (first) {
                    const auto seed_start=std::chrono::steady_clock::now();
                    if (prefixes>=prefix_limit || seed_start>=taxi_deadline) {incomplete=true;return;}
                    // A difficult first pickup must not consume the entire
                    // taxi budget and hide every other entry into its route.
                    // Redistribute unused work from completed branches.
                    prefix_ceiling=prefixes+std::max<std::size_t>(1,(prefix_limit-prefixes)/first_remaining);
                    branch_deadline=taxi_deadline==Deadline::max()?taxi_deadline:
                        seed_start+(taxi_deadline-seed_start)/static_cast<Deadline::duration::rep>(first_remaining);
                }
                const double departure = std::max(arrival,q.ready)+q.service;
                const double expense = cost+in.travel_cost[last*nv+q.pickup];
                picked[r] = 1; served.push_back(r); route.push_back(q.pickup);
                if (r < n) {
                    route.push_back(q.delivery);
                    const auto drop_arrival = departure+in.travel_time[q.pickup*nv+q.delivery];
                    if (std::isfinite(drop_arrival))
                        self(self,q.delivery,drop_arrival+q.service,
                             expense+in.travel_cost[q.pickup*nv+q.delivery], revenue+q.revenue,load,open_count);
                    route.pop_back();
                } else {
                    opened[r] = 1;
                    self(self,q.pickup,departure,expense,revenue+q.revenue,load+q.weight,open_count+1);
                    opened[r] = 0;
                }
                route.pop_back(); served.pop_back(); picked[r] = 0;
                if (first) {incomplete|=exhausted;exhausted=false;--first_remaining;}
                else if (exhausted) return;
            }
        };
        visit(visit,taxi,0,0,0,0,0);
        completed_taxis_ += !exhausted && !incomplete;
        for (const auto& [key,c]:priced_bank) add_column(taxi,key,c.second.route,c.second.profit,false);
    }
}

std::vector<double> RoutePoolRepair::estimate_prices(Deadline deadline) const {
    const auto nr = instance_.passengers.size()+instance_.parcels.size();
    if (prices_revision_==bank_revision_ && cached_prices_.size()==nr) {
        (void)0;
        return cached_prices_;
    }
    std::vector<const Column*> columns;
    std::vector<std::size_t> taxis;
    for (std::size_t t = 0; t < pool_.size(); ++t) for (const auto& [key,c] : pool_[t]) {
        columns.push_back(&c);taxis.push_back(t);
    }

    std::vector<std::size_t> order(columns.size());std::iota(order.begin(),order.end(),0);
    std::sort(order.begin(),order.end(),[&](auto a,auto b) { return columns[a]->profit > columns[b]->profit; });
    std::vector<unsigned char> occupied(nr,0), assigned(pool_.size(),0);
    double lower = 0;
    for (auto j : order) {
        if (assigned[taxis[j]]) continue;
        bool free = true;for (auto r : columns[j]->requests) free &= !occupied[r];
        if (!free) continue;
        assigned[taxis[j]] = 1;for (auto r : columns[j]->requests) occupied[r] = 1;
        lower += columns[j]->profit;
    }
    std::vector<double> prices(nr,0), best_prices = prices;
    double best = std::numeric_limits<double>::infinity();
    for (std::size_t iteration = 0; iteration < 192; ++iteration) {
        std::vector<double> maximum(pool_.size(),0);
        std::vector<const Column*> chosen(pool_.size(),nullptr);
        for (std::size_t j = 0; j < columns.size(); ++j) {
            const auto& c = *columns[j];double value = c.profit;
            for (auto r : c.requests) value -= prices[r];
            if (value > maximum[taxis[j]]) { maximum[taxis[j]] = value;chosen[taxis[j]] = &c; }
        }
        const auto dual = std::accumulate(prices.begin(),prices.end(),0.0)+
                          std::accumulate(maximum.begin(),maximum.end(),0.0);
        if (dual < best) { best = dual;best_prices = prices; }
        std::vector<std::size_t> uses(nr,0);
        for (const auto c : chosen) if (c) for (auto r : c->requests) ++uses[r];
        double norm = 0;
        for (std::size_t r = 0; r < nr; ++r) if (prices[r] > 0 || uses[r] > 1) {
            const auto g = static_cast<double>(uses[r])-1;norm += g*g;
        }
        if (norm == 0 || dual <= lower+1e-7 || !std::isfinite(dual)) break;
        const auto step = 0.7*(dual-lower)/norm;
        for (std::size_t r = 0; r < nr; ++r)
            prices[r] = std::max(0.0,prices[r]+step*(static_cast<double>(uses[r])-1));
    }
    if ((route_master_uses_cplex() || pool_.size()<=96) &&
        std::chrono::steady_clock::now()+std::chrono::milliseconds(250)<deadline) {
        std::vector<MasterColumn> subset;
        for (std::size_t taxi=0;taxi<pool_.size();++taxi) {
            std::vector<std::pair<double,const Column*>> ranked;
            for (const auto& [key,c]:pool_[taxi]) {
                double score=c.profit;for (auto r:key) score-=best_prices[r];ranked.push_back({score,&c});
            }
            const auto count=route_master_uses_cplex()?ranked.size():std::min<std::size_t>(64,ranked.size());
            std::partial_sort(ranked.begin(),ranked.begin()+count,ranked.end(),[](const auto& a,const auto& b){return a.first>b.first;});
            for (std::size_t j=0;j<count;++j) subset.push_back({taxi,ranked[j].second->requests,ranked[j].second->profit});
        }
        const auto lp_deadline=std::min(deadline,std::chrono::steady_clock::now()+std::chrono::seconds(2));
        auto proposed=route_master_prices(subset,pool_.size(),nr,lp_deadline);
        double value=std::accumulate(proposed.begin(),proposed.end(),0.0);
        for (const auto& bank:pool_) {
            double maximum=0;
            for (const auto& [key,c]:bank) {
                double score=c.profit;for (auto r:key) score-=proposed[r];maximum=std::max(maximum,score);
            }
            value+=maximum;
        }
        (void)0;
        if (value<best) best_prices=std::move(proposed);
    }
    return best_prices;
}

Solution RoutePoolRepair::generate_fleets(Deadline deadline, std::shared_ptr<const EmptyTaxiIndex> index,
                                       const Solution* initial, bool refine, bool clean_cut, std::size_t refinement_interval,
                                       bool light_refinement, std::size_t refinement_cycles) {


    const auto nr = instance_.passengers.size()+instance_.parcels.size();
    Solution blank;if (initial) blank=*initial;else blank.routes.resize(pool_.size());SearchState empty(instance_,blank);
    if (!index) index=build_empty_taxi_index(empty,std::min(deadline,std::chrono::steady_clock::now()+std::chrono::seconds(1)));
    empty.set_empty_taxi_index(std::move(index));
    std::vector<std::size_t> requests(nr);std::iota(requests.begin(),requests.end(),0);
    std::mt19937_64 rng(seed_+0x9e3779b97f4a7c15ULL*fleet_calls_++);
    std::size_t builds=0;
    SearchState state=empty,best=empty;PositiveInsertionCache cache;
    RandomRemoval random_removal;RelatedRemoval related_removal;WorstRemoval worst_removal;
    std::vector<std::unique_ptr<Move>> refinement;
    refinement.push_back(std::make_unique<RemoveRequest>());
    refinement.push_back(std::make_unique<AddRequest>());
    if (!light_refinement) {
        refinement.push_back(std::make_unique<PassengerRelocate>());
        refinement.push_back(std::make_unique<ParcelRelocate>());
    }
    if (clean_cut) refinement.push_back(std::make_unique<CleanCut2OptStar>());
    if (!light_refinement) refinement.push_back(std::make_unique<SwapRequest>());
    refinement.push_back(std::make_unique<ReplaceRequest>());
    while (std::chrono::steady_clock::now()<deadline) {
        if (builds) {
            if (builds%8==0 || state.benefit()<best.benefit()*0.995) state.copy_from(best);
            const auto removed=std::max<std::size_t>(4,state.served_count()/12);
            if (builds%4==1) random_removal.apply(state,removed,rng);
            else if (builds%4==2) related_removal.apply(state,removed,rng);
            else if (builds%4==3) worst_removal.apply(state,removed,rng);
            else for (std::size_t i=0,count=2+rng()%std::max<std::size_t>(2,pool_.size()/12);i<count;++i) {
                const auto taxi=rng()%pool_.size();std::vector<std::size_t> removed;
                for (auto r:requests) if (state.location(r).taxi==static_cast<int>(taxi)) removed.push_back(r);
                (void)(!removed.empty() && !state.remove_requests(taxi,removed));
            }
        }
        std::shuffle(requests.begin(),requests.end(),rng);
        for (auto r:requests) {
            if (std::chrono::steady_clock::now()>=deadline) break;
            if (state.location(r).taxi>=0) continue;
            const auto choice=cache.best(state,r);if (!choice || choice->gain<=1e-9) continue;
            (void)(!state.insert(choice->request,choice->taxi,choice->pickup,choice->delivery));
            // Preserve complementary fleet routes even when their individual
            // profit is below the absolute bank floor.
            add(state,choice->taxi,false);
        }
        // A request rejected early in a construction can become profitable
        // after later insertions share its access travel. Fill to a fixed point.
        for (;;) {
            if (std::chrono::steady_clock::now()>=deadline) break;
            std::optional<InsertionChoice> choice;
            for (auto r:requests) if (state.location(r).taxi<0) {
                const auto& option=cache.best(state,r);
                if (option && option->gain>1e-9 && (!choice || option->gain>choice->gain)) choice=option;
            }
            if (!choice) break;
            (void)(!state.insert(choice->request,choice->taxi,choice->pickup,choice->delivery));
            add(state,choice->taxi,false);
        }
        // Expensive refinement is occasional, rather than every proposal.
        if (refine && builds%refinement_interval==0 && std::chrono::steady_clock::now()+std::chrono::milliseconds(250)<deadline) {
            for (std::size_t cycle=0;cycle<refinement_cycles;++cycle) {
                bool changed=false;
                for (std::size_t j=0;j<refinement.size();++j) {
                    changed=refinement[j]->improve(state)||changed;
                }
                if (!changed) break;
            }
            for (auto taxi:state.active_taxis()) add(state,taxi,false);
        }
        if (state.benefit()>best.benefit()+1e-9) {
            best.copy_from(state);
            (void)0;
        }
        ++builds;
    }
    (void)0;
    return best.solution();
}

void RoutePoolRepair::generate_replacements(Deadline deadline, const SearchState& fleet) {
    auto prices=estimate_prices(deadline);
    const double scales[]={1.0,0.8,1.2};const auto scale=scales[pricing_calls_%3];
    for (auto& price:prices) price*=scale;(void)0;
    const auto nr=fleet.request_count(),n=fleet.passenger_count(),k=pool_.size();
    std::mt19937_64 rng(seed_+0x9e3779b97f4a7c15ULL*pricing_calls_++);
    std::size_t moves=0;
    for (std::size_t taxi=0;taxi<k;++taxi) {
        const auto now=std::chrono::steady_clock::now();if (now>=deadline) break;
        const auto taxi_deadline=now+(deadline-now)/static_cast<Deadline::duration::rep>(k-taxi);
        Solution initial;initial.routes.resize(k);initial.routes[taxi]=fleet.cache(taxi).nodes();
        std::vector<std::pair<double,const Column*>> ranked;
        for (const auto& [key,c]:pool_[taxi]) {
            double score=c.profit;for (auto r:key) score-=prices[r];ranked.push_back({score,&c});
        }
        const auto count=std::min<std::size_t>(3,ranked.size());
        std::partial_sort(ranked.begin(),ranked.begin()+count,ranked.end(),[](const auto& a,const auto& b){return a.first>b.first;});
        std::vector<std::vector<std::size_t>> seeds{initial.routes[taxi]};
        for (std::size_t j=0;j<count;++j) seeds.push_back(ranked[j].second->route);
        SearchState state(instance_,initial,fleet.insertion_scratch_handle());
        state.set_empty_taxi_index(fleet.empty_taxi_index());
        std::size_t restarts=0;
        for (std::size_t iteration=0;iteration<48 && std::chrono::steady_clock::now()<taxi_deadline;++iteration) {
            double gain=1e-8;std::optional<InsertionChoice> insertion;std::optional<std::size_t> outgoing;
            std::vector<std::size_t> replacement_nodes;
            for (std::size_t r=0;r<nr;++r) if (state.location(r).taxi<0) {
                if ((r&31)==0 && std::chrono::steady_clock::now()>=taxi_deadline) break;
                const auto choice=best_insertion_on_route(state,r,taxi,state.cache(taxi),gain+prices[r]);
                if (choice && choice->gain-prices[r]>gain) {gain=choice->gain-prices[r];insertion=choice;outgoing.reset();}
            }
            std::vector<std::size_t> served;
            for (std::size_t r=0;r<nr;++r) if (state.location(r).taxi>=0) served.push_back(r);
            for (auto r:served) {
                const auto removal=state.removal_gain(r);
                if (removal && *removal+prices[r]>gain) {
                    gain=*removal+prices[r];outgoing=r;insertion.reset();replacement_nodes.clear();
                }
                if (std::chrono::steady_clock::now()>=taxi_deadline) break;
                const auto loc=state.location(r);auto nodes=state.cache(taxi).nodes();
                nodes.erase(nodes.begin()+loc.dropoff);nodes.erase(nodes.begin()+loc.pickup);
                RouteCache reduced(instance_,taxi,nodes);
                const auto base_gain=removal_savings_cost(instance_,taxi,state.cache(taxi).nodes(),loc.pickup,loc.dropoff)-state.revenue(r);
                for (std::size_t incoming=0;incoming<nr;++incoming) if (state.location(incoming).taxi<0) {
                    if ((incoming&31)==0 && std::chrono::steady_clock::now()>=taxi_deadline) break;
                    const auto choice=best_insertion_on_route(state,incoming,taxi,reduced,gain-base_gain-prices[r]+prices[incoming]);
                    if (choice && choice->gain+base_gain+prices[r]-prices[incoming]>gain) {
                        gain=choice->gain+base_gain+prices[r]-prices[incoming];outgoing=r;insertion=choice;replacement_nodes=nodes;
                    }
                }
            }
            if (gain<=1e-8) {
                if (++restarts>8) break;
                initial.routes[taxi]=seeds[restarts%seeds.size()];
                SearchState restart(instance_,initial,fleet.insertion_scratch_handle());
                restart.set_empty_taxi_index(fleet.empty_taxi_index());state.copy_from(restart);
                served.clear();for (std::size_t r=0;r<nr;++r) if (state.location(r).taxi>=0) served.push_back(r);
                if (!served.empty()) {
                    std::shuffle(served.begin(),served.end(),rng);
                    served.resize(std::min(served.size(),static_cast<std::size_t>(1+rng()%3)));
                    state.remove_requests(taxi,served);
                }
                continue;
            }
            if (outgoing && insertion) {
                const auto r=insertion->request;
                const auto drop=r<n?instance_.passenger_dropoff(r):instance_.parcel_dropoff(r-n);
                replacement_nodes.insert(replacement_nodes.begin()+insertion->delivery,drop);
                replacement_nodes.insert(replacement_nodes.begin()+insertion->pickup,state.pickup_node(r));
                // Validate the completed replacement directly. A nonmetric
                // removal-only shortcut need not be feasible.
                Solution candidate;candidate.routes.resize(k);candidate.routes[taxi]=replacement_nodes;
                SearchState changed(instance_,candidate,fleet.insertion_scratch_handle());
                changed.set_empty_taxi_index(fleet.empty_taxi_index());state.copy_from(changed);
            } else if (outgoing) {
                (void)(!state.remove(*outgoing));
            } else (void)(!state.insert(insertion->request,taxi,insertion->pickup,insertion->delivery));
            add(state,taxi,false);++moves;
        }
    }
    (void)0;
}

void RoutePoolRepair::remember(const SearchState& state) {

    for (const auto taxi : state.active_taxis()) {
        if (versions_[taxi] == state.cache(taxi).version()) continue;
        versions_[taxi] = state.cache(taxi).version();
        add(state, taxi, true);
    }
}

void RoutePoolRepair::set_limit(std::size_t limit) {

    limit_ = limit;
    std::fill(floors_.begin(),floors_.end(),-std::numeric_limits<double>::infinity());
    for (auto& bank : pool_) while (bank.size() > limit_) {
        const auto worst = std::min_element(bank.begin(), bank.end(), [](const auto& a, const auto& b) {
            return a.second.profit < b.second.profit;
        });
        bank.erase(worst);
        ++bank_revision_;
    }
}



bool RoutePoolRepair::improve(SearchState& state, Deadline deadline) {

    // Restore every incumbent route even if the bank's size cap removed it.
    for (const auto taxi : state.active_taxis()) add(state, taxi, false);
    std::vector<std::vector<Column>> columns(pool_.size());
    std::vector<int> selected(pool_.size(), -1), incumbent(pool_.size(), -1);
    std::vector<unsigned char> occupied(state.request_count(), 0);
    std::vector<std::size_t> frequency(state.request_count(), 0), order;
    column_count_ = 0;
    double best_profit = 0;
    for (std::size_t taxi = 0; taxi < pool_.size(); ++taxi) {
        auto& row = columns[taxi];
        for (const auto& [key, column] : pool_[taxi]) row.push_back(column);
        std::sort(row.begin(), row.end(), [](const auto& a, const auto& b) {
            return a.profit > b.profit || (a.profit == b.profit && a.requests < b.requests);
        });
        column_count_ += row.size();
        std::vector<unsigned char> present(state.request_count(), 0);
        for (std::size_t i = 0; i < row.size(); ++i) {
            for (auto r : row[i].requests) present[r] = 1;
            bool current = !row[i].requests.empty();
            for (auto r : row[i].requests) current &= state.location(r).taxi == static_cast<int>(taxi);
            if (current) {
                // The served-set key must equal the entire incumbent route.
                std::size_t count = 0;
                for (std::size_t r = 0; r < state.request_count(); ++r)
                    count += state.location(r).taxi == static_cast<int>(taxi);
                if (count == row[i].requests.size()) { incumbent[taxi] = static_cast<int>(i); best_profit += row[i].profit; }
            }
        }
        for (std::size_t r = 0; r < present.size(); ++r) frequency[r] += present[r];
    }
    // Constrain the taxis with the most shared requests before independent ones.
    std::vector<std::size_t> conflict(pool_.size(), 0);
    for (std::size_t taxi = 0; taxi < columns.size(); ++taxi)
        for (const auto& column : columns[taxi])
            for (auto r : column.requests) conflict[taxi] += frequency[r] - 1;
    for (std::size_t taxi = 0; taxi < columns.size(); ++taxi)
        if (!columns[taxi].empty()) order.push_back(taxi);
    std::stable_sort(order.begin(), order.end(), [&](auto a, auto b) { return conflict[a] > conflict[b]; });
    const auto compatible = [&](const Column& column) {
        for (auto r : column.requests) if (occupied[r]) return false;
        return true;
    };
    nodes_ = 0; timed_out_ = false;
    const auto warmup_start = std::chrono::steady_clock::now();
    const auto warmup_deadline = order.size() > 48 ?
        std::min(deadline,warmup_start+std::chrono::seconds(2)) : deadline;
    // Repair small groups around the feasible incumbent before searching the
    // complete master. This supplies good feasible bounds under a short limit.
    const auto optimize_group = [&](const std::vector<std::size_t>& group) {
        std::fill(occupied.begin(), occupied.end(), 0);
        for (std::size_t taxi = 0; taxi < incumbent.size(); ++taxi) {
            if (incumbent[taxi] < 0 || std::find(group.begin(), group.end(), taxi) != group.end()) continue;
            for (auto r : columns[taxi][incumbent[taxi]].requests) occupied[r] = 1;
        }
        double old_profit = 0;
        std::vector<int> chosen(group.size(), -1), best_group;
        for (auto taxi : group) {
            best_group.push_back(incumbent[taxi]);
            if (incumbent[taxi] >= 0) old_profit += columns[taxi][incumbent[taxi]].profit;
        }
        double group_profit = old_profit;
        std::function<void(std::size_t,double)> visit = [&](std::size_t depth, double profit) {
            if (timed_out_) return;
            ++nodes_;
            if ((nodes_ & 63) == 0 && std::chrono::steady_clock::now() >= warmup_deadline) {
                timed_out_ = true; return;
            }
            if (depth == group.size()) {
                if (profit > group_profit + 1e-9) { group_profit = profit; best_group = chosen; }
                return;
            }
            double bound = profit;
            for (std::size_t d = depth; d < group.size(); ++d)
                for (const auto& column : columns[group[d]])
                    if (compatible(column)) { bound += column.profit; break; }
            if (bound <= group_profit + 1e-9) return;
            const auto taxi = group[depth];
            for (std::size_t i = 0; i < columns[taxi].size(); ++i) {
                const auto& column = columns[taxi][i];
                if (!compatible(column)) continue;
                chosen[depth] = static_cast<int>(i);
                for (auto r : column.requests) occupied[r] = 1;
                visit(depth + 1, profit + column.profit);
                for (auto r : column.requests) occupied[r] = 0;
                if (timed_out_) break;
            }
            chosen[depth] = -1;
            if (!timed_out_) visit(depth + 1, profit);
        };
        if (std::chrono::steady_clock::now() < warmup_deadline) visit(0,0);
        else timed_out_ = true;
        if (group_profit <= old_profit + 1e-9) return false;
        for (std::size_t i = 0; i < group.size(); ++i) incumbent[group[i]] = best_group[i];
        best_profit += group_profit - old_profit;
        return true;
    };
    bool changed = true;
    while (changed && !timed_out_) {
        changed = false;
        for (std::size_t a = 0; a < order.size() && !timed_out_; ++a)
            for (std::size_t b = a + 1; b < order.size() && !timed_out_; ++b)
                changed = optimize_group({order[a],order[b]}) || changed;
    }
    (void)0;
    for (std::size_t a = 0; order.size()<=48 && a < order.size() && !timed_out_; ++a)
        for (std::size_t b = a + 1; b < order.size() && !timed_out_; ++b)
            for (std::size_t c = b + 1; c < order.size() && !timed_out_; ++c)
                optimize_group({order[a],order[b],order[c]});
    (void)0;
    if (column_count_ > 2000) {
        std::vector<MasterColumn> master;
        std::vector<std::pair<std::size_t,std::size_t>> locations;
        std::vector<int> initial(incumbent.size(),-1);
        for (std::size_t taxi = 0; taxi < columns.size(); ++taxi)
            for (std::size_t i = 0; i < columns[taxi].size(); ++i) {
                if (incumbent[taxi] == static_cast<int>(i)) initial[taxi] = static_cast<int>(master.size());
                const auto& c = columns[taxi][i];
                master.push_back({taxi,c.requests,c.profit}); locations.emplace_back(taxi,i);
            }
        const auto guides = columns.size() > 48 && !route_master_uses_cplex() ? estimate_prices() : std::vector<double>{};
        const auto solved = columns.size() > 48 ?
            improve_route_master(master,columns.size(),state.request_count(),initial,deadline,seed_,guides) :
            solve_route_master(master,columns.size(),state.request_count(),initial,deadline);
        if (solved.request_prices.size()==state.request_count()) {
            cached_prices_=solved.request_prices;prices_revision_=bank_revision_;
        }
        best_profit = solved.profit; timed_out_ = solved.timed_out; nodes_ += solved.nodes;
        std::fill(incumbent.begin(),incumbent.end(),-1);
        for (auto id : solved.selected) if (id >= 0) {
            const auto [taxi,index] = locations[id]; incumbent[taxi] = static_cast<int>(index);
        }
        (void)0;
    } else {
    // Nonnegative request prices give a valid Lagrangian upper bound:
    // sum(price) + sum_t max(0, profit(route) - sum_request price).
    // Use projected subgradients to tighten it; every iterate is valid, so
    // convergence is unnecessary for correctness or safe early termination.
    std::vector<double> prices(state.request_count(), 0), best_prices = prices;
    double best_dual = std::numeric_limits<double>::infinity();
    std::vector<std::vector<double>> reduced(columns.size());
    std::vector<std::vector<std::size_t>> branches(columns.size());
    for (std::size_t iteration = 0; iteration < 192 && !timed_out_; ++iteration) {
        if (std::chrono::steady_clock::now() >= deadline) { timed_out_ = true; break; }
        double dual = std::accumulate(prices.begin(), prices.end(), 0.0);
        std::vector<std::size_t> uses(prices.size(), 0);
        for (auto taxi : order) {
            double maximum = 0; int chosen = -1;
            for (std::size_t i = 0; i < columns[taxi].size(); ++i) {
                const auto& column = columns[taxi][i];
                double value = column.profit;
                for (auto r : column.requests) value -= prices[r];
                if (value > maximum) { maximum = value; chosen = static_cast<int>(i); }
            }
            dual += maximum;
            if (chosen >= 0) for (auto r : columns[taxi][chosen].requests) ++uses[r];
        }
        if (dual < best_dual) { best_dual = dual; best_prices = prices; }
        double norm = 0;
        for (std::size_t r = 0; r < prices.size(); ++r)
            if (prices[r] > 0 || uses[r] > 1) {
                const double gradient = static_cast<double>(uses[r]) - 1;
                norm += gradient * gradient;
            }
        if (!norm || dual <= best_profit + 1e-7) break;
        const double step = 0.9 * (dual - best_profit) / norm;
        for (std::size_t r = 0; r < prices.size(); ++r)
            prices[r] = std::max(0.0, prices[r] + step * (static_cast<double>(uses[r]) - 1));
    }
    (void)0;
    prices = std::move(best_prices);
    const double price_total = std::accumulate(prices.begin(), prices.end(), 0.0);
    for (auto taxi : order) {
        auto& values = reduced[taxi];
        for (const auto& column : columns[taxi]) {
            double value = column.profit;
            for (auto r : column.requests) value -= prices[r];
            values.push_back(value);
        }
        auto& indices = branches[taxi];
        indices.resize(values.size()); std::iota(indices.begin(), indices.end(), 0);
        std::stable_sort(indices.begin(), indices.end(), [&](auto a, auto b) { return values[a] > values[b]; });
    }
    std::fill(occupied.begin(), occupied.end(), 0);
    std::function<void(std::size_t,double,double)> search = [&](std::size_t depth, double profit, double price_used) {
        if (timed_out_) return;
        ++nodes_;
        if ((nodes_ & 63) == 0 && std::chrono::steady_clock::now() >= deadline) {
            timed_out_ = true; return;
        }
        if (depth == order.size()) {
            if (profit > best_profit + 1e-9) { best_profit = profit; incumbent = selected; }
            return;
        }
        double bound = profit;
        for (std::size_t d = depth; d < order.size(); ++d)
            for (const auto& column : columns[order[d]])
                if (compatible(column)) { bound += column.profit; break; }
        if (bound <= best_profit + 1e-9) return;
        double priced_bound = profit + price_total - price_used;
        for (std::size_t d = depth; d < order.size(); ++d) {
            const auto taxi = order[d];
            double maximum = 0;
            for (auto i : branches[taxi]) {
                if (reduced[taxi][i] <= 0) break;
                if (compatible(columns[taxi][i])) { maximum = reduced[taxi][i]; break; }
            }
            priced_bound += maximum;
        }
        // Conservatively tolerate floating-point accumulation in the dual.
        if (priced_bound < best_profit - 1e-7) return;
        const auto taxi = order[depth];
        for (auto i : branches[taxi]) {
            const auto& column = columns[taxi][i];
            if (!compatible(column)) continue;
            selected[taxi] = static_cast<int>(i);
            double used = price_used;
            for (auto r : column.requests) { occupied[r] = 1; used += prices[r]; }
            search(depth + 1, profit + column.profit, used);
            for (auto r : column.requests) occupied[r] = 0;
            if (timed_out_) break;
        }
        selected[taxi] = -1;
        if (!timed_out_) search(depth + 1, profit, price_used); // unused taxi is always permitted
    };
    if (std::chrono::steady_clock::now() < deadline) search(0,0,0);
    else timed_out_ = true;

    }
    if (best_profit <= state.benefit() + 1e-9) return false;
    Solution candidate;
    candidate.routes.resize(state.taxi_count());
    for (std::size_t taxi = 0; taxi < incumbent.size(); ++taxi)
        if (incumbent[taxi] >= 0) candidate.routes[taxi] = columns[taxi][incumbent[taxi]].route;
    const auto evaluated = candidate.evaluate(instance_);

    SearchState repaired(instance_, std::move(candidate), state.insertion_scratch_handle());
    repaired.set_empty_taxi_index(state.empty_taxi_index());
    state.copy_from(repaired);
    return true;
}
} // namespace soict::alns

// END src/search/alns/operators/Repair/RoutePoolRepair.cpp

// BEGIN src/search/alns/operators/Repair/RoutePoolMaster.cpp
#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>

namespace soict::alns {
namespace {
struct PackingMatrix {
    const std::vector<MasterColumn>& columns;
    std::vector<std::vector<std::size_t>> rows;
    std::vector<std::size_t> resources;
    std::size_t dimension = 0, taxis;
    PackingMatrix(const std::vector<MasterColumn>& c, std::size_t k, std::size_t requests)
        : columns(c), rows(c.size()), taxis(k) {
        std::vector<int> index(k+requests,-1);
        for (std::size_t j = 0; j < c.size(); ++j) {
            const auto add = [&](std::size_t r) {
                if (index[r] < 0) {index[r] = static_cast<int>(dimension++);resources.push_back(r);}
                rows[j].push_back(index[r]);
            };
            add(c[j].taxi); for (auto r : c[j].requests) add(k+r);
        }
    }
};

// A child adds x_j <= 0 or -x_j <= -1 to its parent's packing LP.
// The parent's dual remains feasible, so dual simplex restores primal
// feasibility while reusing the inverse basis and prices.
class PackingLP {
    const PackingMatrix* matrix_;
    std::size_t n_;
    std::vector<double> inverse_, basic_, prices_, rhs_, signs_;
    std::vector<std::size_t> basis_;
    std::vector<unsigned char> in_basis_;
    std::vector<int> bound_row_;

    double coefficient(std::size_t row, std::size_t column) const {
        const auto nc = matrix_->columns.size();
        if (column >= nc) return inverse_[row*n_+column-nc];
        double value = 0;
        for (auto r : matrix_->rows[column]) value += inverse_[row*n_+r];
        if (bound_row_[column] >= 0) {
            const auto r = static_cast<std::size_t>(bound_row_[column]);
            value += signs_[r]*inverse_[row*n_+r];
        }
        return value;
    }
    double reduced(std::size_t column) const {
        const auto nc = matrix_->columns.size();
        if (column >= nc) return -prices_[column-nc];
        double value = matrix_->columns[column].profit;
        for (auto r : matrix_->rows[column]) value -= prices_[r];
        if (bound_row_[column] >= 0) {
            const auto r = static_cast<std::size_t>(bound_row_[column]);
            value -= signs_[r]*prices_[r];
        }
        return value;
    }
    void pivot(std::size_t leaving, std::size_t entering) {
        std::vector<double> direction(n_), row(n_);
        for (std::size_t i = 0; i < n_; ++i) direction[i] = coefficient(i,entering);
        const auto divisor = direction[leaving];
        const double step = basic_[leaving]/divisor, gain = reduced(entering);
        for (std::size_t r = 0; r < n_; ++r) {
            row[r] = inverse_[leaving*n_+r]/divisor;
            prices_[r] += gain*row[r];
        }
        for (std::size_t i = 0; i < n_; ++i) if (i != leaving) {
            basic_[i] -= step*direction[i];
            if (direction[i] == 0) continue;
            for (std::size_t r = 0; r < n_; ++r) inverse_[i*n_+r] -= direction[i]*row[r];
        }
        std::copy(row.begin(),row.end(),inverse_.begin()+leaving*n_);
        basic_[leaving] = step;
        in_basis_[basis_[leaving]] = 0;
        basis_[leaving] = entering; in_basis_[entering] = 1;
        ++pivots;
    }
public:
    std::size_t pivots = 0;
    bool timed_out = false;
    explicit PackingLP(const PackingMatrix& matrix) : matrix_(&matrix), n_(matrix.dimension),
        inverse_(n_*n_,0), basic_(n_,1), prices_(n_,0), rhs_(n_,1), signs_(n_,0),
        basis_(n_), in_basis_(matrix.columns.size()+n_,0), bound_row_(matrix.columns.size(),-1) {
        for (std::size_t i = 0; i < n_; ++i) {
            inverse_[i*n_+i] = 1;
            basis_[i] = matrix.columns.size()+i; in_basis_[basis_[i]] = 1;
        }
    }
    void primal(Deadline deadline) {
        const auto nc = matrix_->columns.size();
        std::size_t degenerate = 0;
        for (std::size_t iteration = 0; iteration < 10000; ++iteration) {
            if ((iteration & 7) == 0 && std::chrono::steady_clock::now() >= deadline) {
                timed_out = true; return;
            }
            std::size_t entering = nc+n_;
            double gain = 1e-8;
            for (std::size_t j = 0; j < nc+n_; ++j) {
                if (in_basis_[j]) continue;
                const double value = reduced(j);
                if (value > gain) {
                    gain = value; entering = j;
                    if (degenerate > n_) break;
                }
            }
            if (entering == nc+n_) return;
            std::size_t leaving = n_;
            double step = std::numeric_limits<double>::infinity();
            for (std::size_t i = 0; i < n_; ++i) {
                const auto d = coefficient(i,entering);
                if (d <= 1e-9) continue;
                const auto ratio = std::max(0.0,basic_[i])/d;
                if (ratio < step-1e-9 || (std::abs(ratio-step) <= 1e-9 &&
                    (leaving == n_ || basis_[i] < basis_[leaving]))) {
                    leaving = i; step = ratio;
                }
            }
            if (leaving == n_ || !std::isfinite(step)) return;
            degenerate = step < 1e-9 ? degenerate+1 : 0;
            pivot(leaving,entering);
        }
    }
    void constrain(std::size_t column, bool include) {
        const double sign = include ? -1 : 1, rhs = include ? -1 : 0;
        const auto old = n_++;
        std::vector<double> enlarged(n_*n_,0);
        for (std::size_t i = 0; i < old; ++i)
            std::copy_n(inverse_.begin()+i*old,old,enlarged.begin()+i*n_);
        double value = 0;
        for (std::size_t i = 0; i < old; ++i) if (basis_[i] == column) {
            value = basic_[i];
            for (std::size_t r = 0; r < old; ++r) enlarged[old*n_+r] = -sign*inverse_[i*old+r];
            break;
        }
        enlarged[old*n_+old] = 1; inverse_ = std::move(enlarged);
        basic_.push_back(rhs-sign*value); prices_.push_back(0); rhs_.push_back(rhs); signs_.push_back(sign);
        basis_.push_back(matrix_->columns.size()+old); in_basis_.push_back(1);
        bound_row_[column] = static_cast<int>(old); pivots = 0;
    }
    void dual(Deadline deadline) {
        const auto nc = matrix_->columns.size();
        for (std::size_t iteration = 0; iteration < 10000; ++iteration) {
            if ((iteration & 7) == 0 && std::chrono::steady_clock::now() >= deadline) {
                timed_out = true; return;
            }
            std::size_t leaving = n_;
            double minimum = -1e-7;
            for (std::size_t i = 0; i < n_; ++i)
                if (basic_[i] < minimum) { minimum = basic_[i]; leaving = i; }
            if (leaving == n_) return;
            std::size_t entering = nc+n_;
            double ratio = std::numeric_limits<double>::infinity();
            for (std::size_t j = 0; j < nc+n_; ++j) {
                if (in_basis_[j]) continue;
                const auto d = coefficient(leaving,j);
                if (d >= -1e-9) continue;
                const double candidate = reduced(j)/d;
                if (candidate < ratio-1e-9) { entering = j; ratio = candidate; }
            }
            // A numerical stop is never used as an infeasibility proof. The
            // checked price bound remains valid and branching is exhaustive.
            if (entering == nc+n_) return;
            pivot(leaving,entering);
        }
    }
    std::vector<double> values() const {
        std::vector<double> x(matrix_->columns.size(),0);
        for (std::size_t i = 0; i < n_; ++i)
            if (basis_[i] < x.size() && std::isfinite(basic_[i]))
                x[basis_[i]] = std::clamp(basic_[i],0.0,1.0);
        return x;
    }
    double upper() const {
        // Any nonnegative prices give a valid bound, including branch-row
        // prices. Check residual profit for every column independently.
        auto prices = prices_;
        double bound = 0, magnitude = 0;
        for (std::size_t r = 0; r < n_; ++r) {
            if (!std::isfinite(prices[r])) return std::numeric_limits<double>::infinity();
            prices[r] = std::max(0.0,prices[r]);
            bound += prices[r]*rhs_[r]; magnitude += std::abs(prices[r]*rhs_[r]);
        }
        std::vector<double> residual(matrix_->taxis,0);
        for (std::size_t j = 0; j < matrix_->columns.size(); ++j) {
            const auto& c = matrix_->columns[j];
            double value = c.profit;
            for (auto r : matrix_->rows[j]) value -= prices[r];
            if (bound_row_[j] >= 0) {
                const auto r = static_cast<std::size_t>(bound_row_[j]);
                value -= signs_[r]*prices[r];
            }
            residual[c.taxi] = std::max(residual[c.taxi],value);
        }
        const auto remainder = std::accumulate(residual.begin(),residual.end(),0.0);
        bound += remainder; magnitude += remainder;
        if (!std::isfinite(bound) || !std::isfinite(magnitude))
            return std::numeric_limits<double>::infinity();
        return bound+1e-7+1e-10*magnitude;
    }
    std::vector<double> request_prices(std::size_t requests) const {
        std::vector<double> result(requests,0);
        for (std::size_t row=0;row<matrix_->resources.size();++row) {
            const auto resource=matrix_->resources[row];
            if (resource>=matrix_->taxis && std::isfinite(prices_[row]))
                result[resource-matrix_->taxis]=std::max(0.0,prices_[row]);
        }
        return result;
    }
};
}

std::vector<double> route_master_prices(const std::vector<MasterColumn>& columns,
        std::size_t taxis, std::size_t requests, Deadline deadline) {
    const PackingMatrix matrix(columns,taxis,requests);
    PackingLP root(matrix);root.primal(deadline);
    (void)0;
    return root.request_prices(requests);
}

bool route_master_uses_cplex() {
    return false;
}

MasterResult solve_route_master(const std::vector<MasterColumn>& columns,
        std::size_t taxis, std::size_t requests, const std::vector<int>& incumbent,
        Deadline deadline, bool verbose) {
    MasterResult result; result.selected = incumbent;
    for (auto id : incumbent) if (id >= 0) result.profit += columns[id].profit;
    if (columns.empty()) {result.complete=true;return result;}
    const PackingMatrix matrix(columns,taxis,requests);
    PackingLP root(matrix); root.primal(deadline);
    result.pivots = root.pivots;
    if (root.timed_out) { result.timed_out = true; return result; }
    result.root_upper = root.upper();
    if (verbose) (void)0;
    bool integral = true;
    for (const auto& c : columns) integral &= c.profit == std::round(c.profit);
    std::vector<unsigned char> fixed(taxis,0), occupied(requests,0), forbidden(columns.size(),0);
    std::vector<int> selected(taxis,-1);
    const auto compatible = [&](std::size_t id) {
        const auto& c = columns[id];
        if (fixed[c.taxi] || forbidden[id]) return false;
        for (auto r : c.requests) if (occupied[r]) return false;
        return true;
    };
    std::function<void(const PackingLP&,double)> visit = [&](const PackingLP& lp, double profit) {
        if (result.timed_out) return;
        if (std::chrono::steady_clock::now() >= deadline) { result.timed_out = true; return; }
        ++result.nodes;
        double upper = lp.upper(); if (integral) upper = std::floor(upper);
        if (upper <= result.profit+1e-9) return;
        const auto x = lp.values();
        std::vector<std::size_t> ranking;
        for (std::size_t j = 0; j < columns.size(); ++j) if (compatible(j)) ranking.push_back(j);
        if (ranking.empty()) {
            if (profit > result.profit+1e-9) { result.profit = profit; result.selected = selected; }
            return;
        }
        std::stable_sort(ranking.begin(),ranking.end(),[&](auto a,auto b) {
            return x[a] != x[b] ? x[a] > x[b] : columns[a].profit > columns[b].profit;
        });
        auto used = occupied, assigned = fixed; auto fleet = selected;
        double candidate = profit;
        for (auto id : ranking) {
            const auto& c = columns[id];
            if (assigned[c.taxi]) continue;
            bool free = true; for (auto r : c.requests) free &= !used[r];
            if (!free) continue;
            assigned[c.taxi] = 1; for (auto r : c.requests) used[r] = 1;
            fleet[c.taxi] = static_cast<int>(id); candidate += c.profit;
        }
        if (candidate > result.profit+1e-9) {
            result.profit = candidate; result.selected = std::move(fleet);
            if (verbose) (void)0;
        }
        if (upper <= result.profit+1e-9) return;
        std::size_t branch = ranking.front(); double fractionality = -1;
        for (auto j : ranking) {
            if (x[j] <= 1e-7 || x[j] >= 1-1e-7) continue;
            const auto score = std::min(x[j],1-x[j]);
            if (score > fractionality) { fractionality = score; branch = j; }
        }
        const auto& c = columns[branch];
        {
            auto child = lp; child.constrain(branch,true); child.dual(deadline);
            result.pivots += child.pivots;
            if (child.timed_out) { result.timed_out = true; return; }
            fixed[c.taxi] = 1; for (auto r : c.requests) occupied[r] = 1;
            selected[c.taxi] = static_cast<int>(branch); visit(child,profit+c.profit);
            selected[c.taxi] = -1;
            fixed[c.taxi] = 0; for (auto r : c.requests) occupied[r] = 0;
        }
        if (!result.timed_out) {
            auto child = lp; child.constrain(branch,false); child.dual(deadline);
            result.pivots += child.pivots;
            if (child.timed_out) { result.timed_out = true; return; }
            forbidden[branch] = 1; visit(child,profit); forbidden[branch] = 0;
        }
    };
    visit(root,0);
    result.complete = !result.timed_out;
    return result;
}

MasterResult improve_route_master(const std::vector<MasterColumn>& columns,
        std::size_t taxis, std::size_t requests, const std::vector<int>& incumbent,
        Deadline deadline, std::uint64_t seed, const std::vector<double>& request_prices) {
    MasterResult result;result.selected = incumbent;
    result.root_upper = std::numeric_limits<double>::infinity();
    for (auto id : incumbent) if (id >= 0) result.profit += columns[id].profit;
    std::vector<std::vector<std::size_t>> banks(taxis);
    for (std::size_t j = 0; j < columns.size(); ++j) banks[columns[j].taxi].push_back(j);
    std::vector<double> penalties(columns.size(),0);
    if (!request_prices.empty()) for (std::size_t j=0;j<columns.size();++j)
        for (auto r:columns[j].requests) penalties[j]+=request_prices[r];
    // Price-based starts favor routes that leave expensive shared requests
    // available to other taxis. Each constructed fleet is checked directly.
    for (const double scale:{0.0,0.5,1.0,1.5}) {
        std::vector<std::size_t> ranking(columns.size());std::iota(ranking.begin(),ranking.end(),0);
        std::sort(ranking.begin(),ranking.end(),[&](auto a,auto b) {
            const auto x=columns[a].profit-scale*penalties[a],y=columns[b].profit-scale*penalties[b];
            return x!=y?x>y:a<b;
        });
        std::vector<unsigned char> occupied(requests,0);std::vector<int> fleet(taxis,-1);double profit=0;
        for (auto j:ranking) {
            const auto& c=columns[j];if (fleet[c.taxi]>=0) continue;
            bool free=true;for (auto r:c.requests) free&=!occupied[r];if (!free) continue;
            fleet[c.taxi]=static_cast<int>(j);for (auto r:c.requests) occupied[r]=1;profit+=c.profit;
        }
        if (profit>result.profit+1e-9) {result.profit=profit;result.selected=std::move(fleet);
            (void)0;}
    }
    for (auto& bank:banks) std::sort(bank.begin(),bank.end(),[&](auto a,auto b) {
        const auto x=columns[a].profit-penalties[a],y=columns[b].profit-penalties[b];
        return x!=y?x>y:a<b;
    });
    std::mt19937_64 rng(seed);
    const auto start = std::chrono::steady_clock::now();
    std::size_t rounds = 0, last_gain = 0;
    while (std::chrono::steady_clock::now() < deadline) {
        std::vector<int> owner(requests,-1);
        for (std::size_t t = 0; t < taxis; ++t) if (result.selected[t] >= 0)
            for (auto r : columns[result.selected[t]].requests) owner[r] = static_cast<int>(t);
        const std::size_t width = std::min(taxis,static_cast<std::size_t>(8+4*((rounds/taxis)%4)));
        std::vector<std::size_t> group;
        std::vector<unsigned char> inside(taxis,0);
        const auto add = [&](std::size_t t) { if (!inside[t]) { group.push_back(t);inside[t] = 1; } };
        const auto first = rounds%taxis; add(first);
        // Follow the owners blocking a promising alternative. Periodic
        // randomization diversifies the coordinated neighborhood.
        if (!banks[first].empty()) {
            const auto rank = rounds%3 == 0 ? rng()%std::min<std::size_t>(24,banks[first].size()) : 0;
            for (auto r : columns[banks[first][rank]].requests)
                if (owner[r] >= 0 && group.size() < width) add(owner[r]);
        }
        while (group.size() < width) {
            std::vector<double> score(taxis,0);
            for (auto t : group) {
                const auto old = result.selected[t] >= 0 ? columns[result.selected[t]].profit : 0;
                const auto count = std::min<std::size_t>(32,banks[t].size());
                for (std::size_t j = 0; j < count; ++j) {
                    const auto& c = columns[banks[t][j]];
                    const auto value = std::max(1.0,c.profit-old);
                    for (auto r : c.requests) if (owner[r] >= 0 && !inside[owner[r]]) score[owner[r]] += value;
                }
            }
            std::size_t chosen = taxis;double maximum = 0;
            for (std::size_t t = 0; t < taxis; ++t) if (!inside[t] && score[t] > maximum) {
                maximum = score[t]; chosen = t;
            }
            if (chosen == taxis || rounds%5 == 4) {
                do { chosen = rng()%taxis; } while (inside[chosen]);
            }
            add(chosen);
        }
        std::vector<MasterColumn> local;
        std::vector<std::size_t> global;
        std::vector<int> initial(group.size(),-1);
        for (std::size_t t = 0; t < group.size(); ++t) for (auto id : banks[group[t]]) {
            const auto& c = columns[id];bool available = true;
            for (auto r : c.requests) if (owner[r] >= 0 && !inside[owner[r]]) { available = false; break; }
            if (!available) continue;
            if (result.selected[group[t]] == static_cast<int>(id)) initial[t] = static_cast<int>(local.size());
            local.push_back({t,c.requests,c.profit});global.push_back(id);
        }
        const auto now = std::chrono::steady_clock::now();
        const auto local_deadline = std::min(deadline,now+std::chrono::milliseconds(150));
        const auto solved = solve_route_master(local,group.size(),requests,initial,local_deadline,false);
        result.nodes += solved.nodes;result.pivots += solved.pivots;
        double old = 0;for (auto id : initial) if (id >= 0) old += local[id].profit;
        if (solved.profit > old+1e-9) {
            for (std::size_t t = 0; t < group.size(); ++t)
                result.selected[group[t]] = solved.selected[t] >= 0 ? static_cast<int>(global[solved.selected[t]]) : -1;
            result.profit += solved.profit-old;last_gain = rounds;
            (void)0;
        }
        ++rounds;
        // Return a useful incumbent instead of spending the entire allowance
        // on identical neighborhoods after several unsuccessful full sweeps.
        if (rounds-last_gain > 12*taxis) break;
    }
    (void)0;
    result.timed_out = std::chrono::steady_clock::now()>=deadline;
    return result;
}
}

// END src/search/alns/operators/Repair/RoutePoolMaster.cpp

// BEGIN src/search/alns/operators/LocalSearch/LocalSearch.cpp

namespace soict::alns {
bool LocalSearch::search(SearchState& state, std::chrono::steady_clock::time_point deadline) const {
    bool changed = false;
    for (int cycle = 0; cycle < 2; ++cycle) {
        bool cycle_changed = false;
        for (const auto& move : moves_) {
            if (std::chrono::steady_clock::now() >= deadline) return changed || cycle_changed;
            cycle_changed = move->improve(state) || cycle_changed;
        }
        if (!cycle_changed) break;
        changed = true;
    }
    return changed;
}
} // namespace soict::alns

// END src/search/alns/operators/LocalSearch/LocalSearch.cpp

// BEGIN src/search/alns/operators/LocalSearch/AddRequest.cpp

#include <optional>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace soict::alns {
bool AddRequest::improve(SearchState& state) const {
    if (requests_.size() != state.request_count()) {
        requests_.resize(state.request_count());
        std::iota(requests_.begin(), requests_.end(), 0);
    }
    cache_.prepare(state, requests_);
    bool changed = false;
    for (;;) {
        std::optional<InsertionChoice> best;
        for (const auto request : requests_) {
            if (state.location(request).taxi >= 0) continue;
            const auto move = cache_.best(request);
            if (move && move->gain > 1e-9 &&
                (!best || move->gain > best->gain)) best = move;
        }
        if (!best) break;
        (void)(!state.insert(best->request, best->taxi, best->pickup, best->delivery));
        changed = true;
        cache_.refresh_taxi(state, best->taxi, requests_);
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
        (void)(!state.remove(*best));
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
    // Later delivery positions cannot repair a violation in this fixed prefix.
    if (!candidate.feasible()) return std::nullopt;
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

// BEGIN src/search/alns/operators/LocalSearch/CleanCut2OptStar.cpp

#include <stdexcept>
#include <vector>

namespace soict::alns {
namespace {
struct RouteCuts {
    std::size_t taxi;
    std::vector<std::size_t> boundaries;
};

std::vector<std::size_t> clean_boundaries(const SearchState& state, std::size_t taxi) {
    const auto& in = state.instance();
    const auto& route = state.cache(taxi);
    const auto k = in.taxis.size(), n = in.passengers.size(), m = in.parcels.size();
    std::size_t open_parcels = 0;
    std::vector<std::size_t> cuts{0};
    for (std::size_t pos = 0; pos < route.nodes().size(); ++pos) {
        const auto node = route.nodes()[pos];
        if (node >= k + 2 * n && node < k + 2 * n + m) ++open_parcels;
        else if (node >= k + 2 * n + m) --open_parcels;
        if (!open_parcels && route.boundary_allowed(in, pos + 1)) cuts.push_back(pos + 1);
    }
    return cuts;
}
} // namespace

bool CleanCut2OptStar::improve(SearchState& state) const {
    const auto& in = state.instance();
    bool changed = false;
    for (;;) {
        std::vector<RouteCuts> routes;
        routes.reserve(state.active_taxis().size());
        for (const auto taxi : state.active_taxis())
            routes.push_back({taxi, clean_boundaries(state, taxi)});
        bool applied = false;
        for (std::size_t i = 0; i < routes.size() && !applied; ++i) {
            for (std::size_t j = i + 1; j < routes.size() && !applied; ++j) {
                const auto taxi_a = routes[i].taxi, taxi_b = routes[j].taxi;
                const auto& a = state.cache(taxi_a);
                const auto& b = state.cache(taxi_b);
                const auto length_a = a.nodes().size(), length_b = b.nodes().size();
                double best_gain = 1e-9;
                std::size_t best_a = 0, best_b = 0;
                for (const auto cut_a : routes[i].boundaries) {
                    for (const auto cut_b : routes[j].boundaries) {
                        const auto prev_a = cut_a ? a.nodes()[cut_a - 1] : taxi_a;
                        const auto prev_b = cut_b ? b.nodes()[cut_b - 1] : taxi_b;
                        double gain = 0;
                        if (cut_a < length_a) {
                            const auto next_a = a.nodes()[cut_a];
                            gain += in.cost(prev_a, next_a) - in.cost(prev_b, next_a);
                        }
                        if (cut_b < length_b) {
                            const auto next_b = b.nodes()[cut_b];
                            gain += in.cost(prev_b, next_b) - in.cost(prev_a, next_b);
                        }
                        if (gain <= best_gain + 1e-8) continue;
                        FastRouteCheck new_a(in, taxi_a, a);
                        new_a.range(0, cut_a);
                        if (cut_b < length_b) new_a.segment(b.range(in, cut_b, length_b - 1));
                        if (!new_a.feasible()) continue;
                        FastRouteCheck new_b(in, taxi_b, b);
                        new_b.range(0, cut_b);
                        if (cut_a < length_a) new_b.segment(a.range(in, cut_a, length_a - 1));
                        if (!new_b.feasible()) continue;
                        best_gain = gain;
                        best_a = cut_a;
                        best_b = cut_b;
                    }
                }
                if (best_gain > 1e-9) {
                    (void)(!state.exchange_route_suffixes(taxi_a, best_a, taxi_b, best_b));
                    changed = applied = true;
                }
            }
        }
        if (!applied) return changed;
    }
}
} // namespace soict::alns

// END src/search/alns/operators/LocalSearch/CleanCut2OptStar.cpp

// BEGIN src/search/alns/operators/LocalSearch/ReplaceRequest.cpp

#include <algorithm>
#include <stdexcept>
#include <tuple>

namespace soict::alns {
std::optional<ReplacementChoice> ReplaceRequest::best_choice(const SearchState& state) const {
    synchronize(state);
    discard_stale();
    return queue_.empty() ? std::nullopt : std::optional(queue_.top().choice);
}

void ReplaceRequest::discard_stale() const {
    while (!queue_.empty()) {
        const auto& entry = queue_.top();
        const auto taxi = entry.choice.insertion.taxi;
        const auto incoming = entry.choice.insertion.request;
        if (unserved_[incoming] && entry.route_version == route_versions_[taxi] &&
            entry.generation == descriptors_[taxi][incoming].generation) break;
        queue_.pop();
    }
}

bool ReplaceRequest::LowerPriority::operator()(const QueuedChoice& a,
                                              const QueuedChoice& b) const {
    if (a.choice.gain != b.choice.gain) return a.choice.gain < b.choice.gain;
    const auto key = [](const ReplacementChoice& c) {
        return std::tuple(c.outgoing, c.insertion.request,
                          c.insertion.pickup, c.insertion.delivery);
    };
    // Match the original outgoing/incoming/position traversal on equal gains.
    return key(a.choice) > key(b.choice);
}

void ReplaceRequest::refresh_descriptors(const SearchState& state, std::size_t taxi,
    const std::vector<std::size_t>& outgoing,
    const std::vector<std::size_t>& incoming) const {
    if (incoming.empty()) return;
    auto& row = descriptors_[taxi];
    for (const auto request : incoming) {
        ++row[request].generation;
        row[request].choice.reset();
    }
    const auto& original = state.cache(taxi);
    std::vector<std::size_t> nodes;
    for (const auto request : outgoing) {
        const auto loc = state.location(request);
        auto& cached = rows_[request];
        if (cached.reduced_version != original.version()) {
            nodes = original.nodes();
            nodes.erase(nodes.begin() + loc.dropoff);
            nodes.erase(nodes.begin() + loc.pickup);
            cached.reduced.rebuild(state.instance(), taxi, nodes);
            cached.removal_gain = removal_savings_cost(state.instance(), taxi,
                original.nodes(), loc.pickup, loc.dropoff) - state.revenue(request);
            cached.reduced_version = original.version();
        }
        for (const auto replacement : incoming) {
            auto& descriptor = row[replacement];
            const double threshold = descriptor.choice ? descriptor.choice->gain : 1e-9;
            const auto insertion = best_insertion_on_route(state, replacement, taxi,
                cached.reduced, threshold - cached.removal_gain);
            if (insertion && insertion->gain + cached.removal_gain > threshold)
                descriptor.choice = ReplacementChoice{request, *insertion,
                                                       insertion->gain + cached.removal_gain};
        }
    }
    for (const auto request : incoming) enqueue(taxi, request);
}

void ReplaceRequest::enqueue(std::size_t taxi, std::size_t incoming) const {
    const auto& descriptor = descriptors_[taxi][incoming];
    if (descriptor.choice) {
        queue_.push({*descriptor.choice, route_versions_[taxi], descriptor.generation});
    }
}

void ReplaceRequest::synchronize(const SearchState& state) const {
    const auto taxis = state.taxi_count(), requests = state.request_count();
    if (smd_instance_ != &state.instance() || route_versions_.size() != taxis ||
        unserved_.size() != requests) {
        smd_instance_ = &state.instance();
        route_versions_.assign(taxis, std::numeric_limits<std::uint64_t>::max());
        unserved_.assign(requests, false);
        descriptors_.assign(taxis, std::vector<Descriptor>(requests));
        queue_ = Queue{};
        rows_.resize(requests);
    }
    std::vector<std::vector<std::size_t>> outgoing(taxis);
    std::vector<std::size_t> incoming, newly_unserved;
    for (std::size_t request = 0; request < requests; ++request) {
        const auto loc = state.location(request);
        const bool unserved = loc.taxi < 0;
        if (unserved) {
            incoming.push_back(request);
            if (!unserved_[request]) newly_unserved.push_back(request);
        } else outgoing[loc.taxi].push_back(request);
        unserved_[request] = unserved;
    }
    for (std::size_t taxi = 0; taxi < taxis; ++taxi) {
        const auto version = state.cache(taxi).version();
        if (route_versions_[taxi] != version) {
            route_versions_[taxi] = version;
            // Timing and load changes can affect every position on this route.
            refresh_descriptors(state, taxi, outgoing[taxi], incoming);
        } else if (!newly_unserved.empty()) {
            // Unchanged routes only gain candidates for newly unserved requests.
            refresh_descriptors(state, taxi, outgoing[taxi], newly_unserved);
        }
    }
    // Lazy invalidation retains obsolete queue entries briefly. Bound storage
    // across long ALNS runs, including restored/rejected candidate states.
    if (queue_.size() > 2 * taxis * requests + 32) compact_queue();
}

void ReplaceRequest::compact_queue() const {
    queue_ = Queue{};
    for (std::size_t taxi = 0; taxi < descriptors_.size(); ++taxi)
        for (std::size_t incoming = 0; incoming < unserved_.size(); ++incoming) {
            if (unserved_[incoming]) enqueue(taxi, incoming);
        }
}



bool ReplaceRequest::improve(SearchState& state) const {
    bool changed = false;
    while (const auto choice = best_choice(state)) {
        (void)(!state.replace_request(choice->outgoing, choice->insertion.request,
                                   choice->insertion.pickup, choice->insertion.delivery));
        changed = true;
    }
    return changed;
}
} // namespace soict::alns

// END src/search/alns/operators/LocalSearch/ReplaceRequest.cpp

// BEGIN src/search/alns/operators/LocalSearch/SwapRequest.cpp

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <tuple>

namespace soict::alns {
namespace {
bool better(const RequestSwapChoice& a, const RequestSwapChoice& b) {
    return a.gain > b.gain || (a.gain == b.gain &&
        std::tie(a.first, a.second) < std::tie(b.first, b.second));
}

std::optional<double> swapped_route_cost(const SearchState& state,
                                        std::size_t outgoing, std::size_t incoming) {
    const auto loc = state.location(outgoing);
    const auto taxi = static_cast<std::size_t>(loc.taxi);
    const auto& route = state.cache(taxi);
    const auto& in = state.instance();
    const bool passenger = incoming < state.passenger_count();
    const auto delivery = passenger ? in.passenger_dropoff(incoming) :
        in.parcel_dropoff(incoming - state.passenger_count());
    RouteCandidate candidate(in, taxi, route);
    candidate.add_range(0, loc.pickup);
    candidate.add_node(state.pickup_node(incoming));
    if (passenger) candidate.add_node(delivery);
    candidate.add_range(loc.pickup + 1, loc.dropoff);
    if (!passenger) candidate.add_node(delivery);
    candidate.add_range(loc.dropoff + 1, route.nodes().size());
    return candidate.cost_if_feasible();
}
} // namespace

std::optional<double> request_swap_gain(const SearchState& state,
                                       std::size_t first, std::size_t second) {
    if (first >= state.request_count() || second >= state.request_count()) return std::nullopt;
    if (second < first) std::swap(first, second);
    const auto a = state.location(first), b = state.location(second);
    if (a.taxi < 0 || b.taxi < 0 || a.taxi == b.taxi) return std::nullopt;
    const auto cost_a = swapped_route_cost(state, first, second);
    if (!cost_a) return std::nullopt;
    const auto cost_b = swapped_route_cost(state, second, first);
    if (!cost_b) return std::nullopt;
    return state.cache(a.taxi).cost() + state.cache(b.taxi).cost() - *cost_a - *cost_b;
}

bool SwapRequest::LowerPriority::operator()(const QueuedChoice& a, const QueuedChoice& b) const {
    return better(b.choice, a.choice);
}

void SwapRequest::enqueue(std::size_t taxi_a, std::size_t taxi_b) const {
    const auto& descriptor = descriptors_[taxi_a][taxi_b];
    if (descriptor.choice)
        queue_.push({*descriptor.choice, taxi_a, taxi_b,
                     versions_[taxi_a], versions_[taxi_b], descriptor.generation});
}

void SwapRequest::synchronize(const SearchState& state) const {
    const auto taxis = state.taxi_count();
    if (instance_ != &state.instance() || versions_.size() != taxis) {
        instance_ = &state.instance();
        versions_.assign(taxis, std::numeric_limits<std::uint64_t>::max());
        descriptors_.assign(taxis, std::vector<Descriptor>(taxis));
        queue_ = Queue{};
    }
    std::vector<std::size_t> changed;
    std::vector<bool> dirty(taxis, false);
    for (std::size_t taxi = 0; taxi < taxis; ++taxi) {
        const auto version = state.cache(taxi).version();
        if (versions_[taxi] != version) {
            versions_[taxi] = version;
            changed.push_back(taxi);
            dirty[taxi] = true;
        }
    }
    if (changed.empty()) return;
    std::vector<std::vector<std::size_t>> requests(taxis);
    for (std::size_t request = 0; request < state.request_count(); ++request) {
        const auto taxi = state.location(request).taxi;
        if (taxi >= 0) requests[taxi].push_back(request);
    }
    for (const auto taxi : changed) {
        for (std::size_t other = 0; other < taxis; ++other) {
            if (other == taxi || (dirty[other] && other < taxi)) continue;
            const auto taxi_a = std::min(taxi, other), taxi_b = std::max(taxi, other);
            auto& descriptor = descriptors_[taxi_a][taxi_b];
            ++descriptor.generation;
            descriptor.choice.reset();
            for (const auto first : requests[taxi_a]) {
                for (const auto second : requests[taxi_b]) {
                    const auto gain = request_swap_gain(state, first, second);
                    if (!gain || *gain <= 1e-9) continue;
                    const RequestSwapChoice choice{std::min(first, second),
                                                  std::max(first, second), *gain};
                    if (!descriptor.choice || better(choice, *descriptor.choice))
                        descriptor.choice = choice;
                }
            }
            enqueue(taxi_a, taxi_b);
        }
    }
    if (queue_.size() > 2 * taxis * taxis + 32) {
        queue_ = Queue{};
        for (std::size_t a = 0; a < taxis; ++a)
            for (std::size_t b = a + 1; b < taxis; ++b) enqueue(a, b);
    }
}

std::optional<RequestSwapChoice> SwapRequest::best_choice(const SearchState& state) const {

    synchronize(state);
    while (!queue_.empty()) {
        const auto& entry = queue_.top();
        if (entry.version_a == versions_[entry.taxi_a] &&
            entry.version_b == versions_[entry.taxi_b] &&
            entry.generation == descriptors_[entry.taxi_a][entry.taxi_b].generation)
            return entry.choice;
        queue_.pop();
    }
    return std::nullopt;
}

bool SwapRequest::improve(SearchState& state) const {
    bool changed = false;
    while (const auto choice = best_choice(state)) {
        (void)(!state.swap_requests(choice->first, choice->second));
        changed = true;
    }
    return changed;
}
} // namespace soict::alns

// END src/search/alns/operators/LocalSearch/SwapRequest.cpp
#include <iostream>
#include <chrono>

// Judge entry point. Include input parsing in the five-minute wall budget.
int main() {
    const auto started = std::chrono::steady_clock::now();
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
}
