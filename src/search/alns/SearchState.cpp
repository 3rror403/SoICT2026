#include "search/alns/SearchState.hpp"
#include "search/alns/FastRouteCheck.hpp"
#include "search/alns/MoveCost.hpp"

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
    rebuild_route(taxi, route);
    benefit_ += gain;
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
    return true;
}

void SearchState::check() const {
    const auto result = solution().evaluate(*instance_);
    if (!result.feasible || std::abs(result.benefit - benefit_) >
                               1e-6 * std::max(1.0, std::abs(result.benefit)))
        throw std::logic_error("search-state objective or feasibility mismatch: " + result.error);
}
} // namespace soict::alns
