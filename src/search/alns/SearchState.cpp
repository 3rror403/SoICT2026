#include "search/alns/SearchState.hpp"
#include "search/alns/InsertionScratch.hpp"
#include "search/alns/FastRouteCheck.hpp"
#include "search/alns/MoveCost.hpp"
#include "search/alns/RequestData.hpp"

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
    if (insertion_scratch_->request_count() != locations_.size())
        throw std::logic_error("insertion scratch request count mismatch");
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

void SearchState::swap(SearchState& other) noexcept {
    using std::swap;
    swap(instance_, other.instance_);
    insertion_scratch_.swap(other.insertion_scratch_);
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
    for (const auto node : cache(taxi).nodes()) locations_[request_id(*instance_, node)] = {};
    auto& slot = routes_by_taxi_.at(taxi);
    if (route.empty()) {
        slot.clear();
        set_active(taxi, false);
    } else {
        slot.rebuild(in, taxi, route);
        set_active(taxi, true);
    }
    locate_route(taxi);
}

void SearchState::set_active(std::size_t taxi, bool active) {
    const auto where = std::lower_bound(active_taxis_.begin(), active_taxis_.end(), taxi);
    const bool was_active = where != active_taxis_.end() && *where == taxi;
    if (active && !was_active) active_taxis_.insert(where, taxi);
    else if (!active && was_active) active_taxis_.erase(where);
}

void SearchState::locate_route(std::size_t taxi) {
    const auto& route = cache(taxi).nodes();
    for (std::size_t p = 0; p < route.size(); ++p) {
        const auto node = route[p], request = request_id(*instance_, node);
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

bool SearchState::remove_requests(std::size_t taxi, std::span<const std::size_t> requests) {
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
    auto& slot = routes_by_taxi_.at(taxi);
    for (const auto node : slot.nodes()) locations_[request_id(*instance_, node)] = {};
    const bool active = !updated.nodes().empty();
    // Keep the validated version and reuse the old route's buffers next time.
    std::swap(slot, updated);
    set_active(taxi, active);
    if (!active) slot.clear();
    locate_route(taxi);
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
        if (updated.nodes().empty()) {
            routes_by_taxi_[taxi].clear();
            set_active(taxi, false);
        } else {
            routes_by_taxi_[taxi] = std::move(updated);
            set_active(taxi, true);
        }
        locate_route(taxi);
    };
    assign(taxi_a, std::move(new_a));
    assign(taxi_b, std::move(new_b));
    benefit_ += gain;
}

void SearchState::check() const {
    const auto result = solution().evaluate(*instance_);
    if (!result.feasible || std::abs(result.benefit - benefit_) >
                               1e-6 * std::max(1.0, std::abs(result.benefit)))
        throw std::logic_error("search-state objective or feasibility mismatch: " + result.error);
}
} // namespace soict::alns
