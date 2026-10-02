#include "search/alns/RouteCache.hpp"

#include <algorithm>
#include <atomic>
#include <bit>
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
    thread_local std::vector<Segment> cache;
    thread_local const Instance* cached_instance = nullptr;
    
    if (cached_instance != &in) {
        cached_instance = &in;
        cache.clear();
        cache.resize(in.node_count());
        for (std::size_t i = 0; i < in.node_count(); ++i) {
            Segment s;
            s.first = s.last = i;
            const auto k = in.taxis.size(), n = in.passengers.size(), m = in.parcels.size();
            
            if (i < k) {
                s.duration = 0;
                s.load_change = 0;
                s.min_load = 0;
                s.max_load = 0;
            } else {
                s.duration = service(in, i);
                const Window* window = nullptr;
                if (i < k + n) window = &in.passengers[i - k].pickup;
                else if (i >= k + 2 * n && i < k + 2 * n + m)
                    window = &in.parcels[i - k - 2 * n].pickup;
                else if (i >= k + 2 * n + m)
                    window = &in.parcels[i - k - 2 * n - m].delivery;
                if (window) {
                    s.ready = window->earliest + s.duration;
                    s.latest_input = window->latest;
                }
                s.load_change = load_delta(in, i);
                s.min_load = std::min(0.0, s.load_change);
                s.max_load = std::max(0.0, s.load_change);
            }
            cache[i] = s;
        }
    }
    return cache[node];
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
    const auto level = std::bit_width(first ^ last) - 1;
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
