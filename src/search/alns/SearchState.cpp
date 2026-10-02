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

bool SearchState::insert_block(std::size_t taxi, std::size_t gap,
                               const std::array<std::size_t, 4>& nodes,
                               double expected_gain) {
    auto route = cache(taxi).nodes();
    route.insert(route.begin() + gap, nodes.begin(), nodes.end());
    
    RouteCache changed(*instance_, taxi, route);
    RouteCandidate candidate(*instance_, taxi, changed);
    candidate.add_range(0, route.size());
    const auto cost = candidate.cost_if_feasible();
    if (!cost) return false;
    
    benefit_ += expected_gain;
    rebuild_route(taxi, std::move(route));
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

bool SearchState::apply_clean_cut(std::size_t taxiA, std::size_t cutA,
                                  std::size_t taxiB, std::size_t cutB, double delta_benefit) {
    if (!is_active(taxiA) || !is_active(taxiB)) return false;
    const auto& in = *instance_;
    const auto n = in.passengers.size(), m = in.parcels.size(), k_count = in.taxis.size();
    
    const auto old_nodesA = cache(taxiA).nodes();
    const auto old_nodesB = cache(taxiB).nodes();
    if (cutA > old_nodesA.size() || cutB > old_nodesB.size()) return false;
    
    auto clear_locs = [&](const std::vector<std::size_t>& nodes) {
        for (const auto node : nodes) {
            const auto request = node < k_count + 2 * n ? (node - k_count) % n : n + (node - k_count - 2 * n) % m;
            locations_[request] = {};
        }
    };
    clear_locs(old_nodesA);
    clear_locs(old_nodesB);
    
    std::vector<std::size_t> newA, newB;
    newA.reserve(cutA + old_nodesB.size() - cutB);
    newB.reserve(cutB + old_nodesA.size() - cutA);
    
    newA.insert(newA.end(), old_nodesA.begin(), old_nodesA.begin() + cutA);
    newA.insert(newA.end(), old_nodesB.begin() + cutB, old_nodesB.end());
    
    newB.insert(newB.end(), old_nodesB.begin(), old_nodesB.begin() + cutB);
    newB.insert(newB.end(), old_nodesA.begin() + cutA, old_nodesA.end());
    
    benefit_ += delta_benefit;
    
    auto assign_route = [&](std::size_t taxi, std::vector<std::size_t> route) {
        if (route.empty()) {
            std::erase(active_taxis_, taxi);
            routes_by_taxi_[taxi] = RouteCache();
        } else {
            if (!is_active(taxi)) {
                active_taxis_.push_back(taxi);
            }
            routes_by_taxi_[taxi] = RouteCache(*instance_, taxi, route);
        }
        for (std::size_t p = 0; p < route.size(); ++p) {
            const auto node = route[p];
            const bool passenger = node < k_count + 2 * n;
            const auto request = passenger ? (node - k_count) % n : n + (node - k_count - 2 * n) % m;
            auto& loc = locations_[request];
            loc.taxi = static_cast<int>(taxi);
            if (node == pickup_node(request)) loc.pickup = p;
            else loc.dropoff = p;
        }
    };
    
    assign_route(taxiA, std::move(newA));
    assign_route(taxiB, std::move(newB));
    return true;
}

void SearchState::check() const {
    const auto result = solution().evaluate(*instance_);
    if (!result.feasible || std::abs(result.benefit - benefit_) >
                               1e-6 * std::max(1.0, std::abs(result.benefit)))
        throw std::logic_error("search-state objective or feasibility mismatch: " + result.error);
}
} // namespace soict::alns
