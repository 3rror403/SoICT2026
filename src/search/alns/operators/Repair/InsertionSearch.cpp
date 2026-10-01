#include "search/alns/operators/Repair/InsertionSearch.hpp"
#include "search/alns/FastRouteCheck.hpp"

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
void consider(InsertionOptions& result, InsertionChoice choice, std::size_t K = 3) {
    auto& top = result.global_top;
    if (std::find(top.begin(), top.end(), choice) != top.end()) return;
    top.push_back(choice);
    std::sort(top.begin(), top.end(), [](const auto& a, const auto& b) {
        return a.gain > b.gain || (a.gain == b.gain && a.taxi < b.taxi);
    });
    if (top.size() > K) top.resize(K);
}

void combine(InsertionOptions& result, const InsertionOptions& route, std::size_t K = 3) {
    if (route.global_top.empty()) return;
    for (const auto& choice : route.global_top) consider(result, choice, K);
    
    auto& rb = result.route_best;
    rb.push_back(route.global_top[0]);
    std::sort(rb.begin(), rb.end(), [](const auto& a, const auto& b) {
        return a.gain > b.gain || (a.gain == b.gain && a.taxi < b.taxi);
    });
    if (rb.size() > K) rb.resize(K);
}

bool add_empty_taxis(InsertionOptions& result, const SearchState& state,
                     std::size_t request) {
    const auto& index = state.empty_taxi_index();
    const bool indexed = index && index->ensure(request);
    if (indexed) {
        std::size_t found = 0;
        for (const auto& empty : index->choices(request)) {
            if (state.is_active(empty.taxi)) continue;
            InsertionChoice choice{request, empty.taxi, 0, 0, empty.gain};
            consider(result, choice);
            result.route_best.push_back(choice);
            if (++found == 3) break;
        }
        std::sort(result.route_best.begin(), result.route_best.end(), [](const auto& a, const auto& b) {
            return a.gain > b.gain || (a.gain == b.gain && a.taxi < b.taxi);
        });
        if (result.route_best.size() > 3) result.route_best.resize(3);
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
    
    struct PairCandidate {
        double delta;
        std::size_t a, b;
    };
    thread_local std::vector<PairCandidate> pairs;
    pairs.clear();
    
    for (std::size_t a = 0; a <= length; ++a) {
        if (!route.boundary_allowed(in, a)) continue;
        const auto threshold = [&] { return std::max(global_second, result.second_gain()); };
        
        if (data.passenger) {
            const auto delta = adjacent_delta[a];
            if (data.revenue - delta + 1e-8 >= threshold()) {
                pairs.push_back({delta, a, a});
            }
        } else {
            const auto best_delta = std::min(adjacent_delta[a], pickup_delta[a] + suffix_delivery[a + 1]);
            if (data.revenue - best_delta + 1e-8 < threshold()) continue;
            for (std::size_t b = a; b <= length; ++b) {
                if (!route.boundary_allowed(in, b)) continue;
                const auto delta = b == a ? adjacent_delta[a] : pickup_delta[a] + delivery_delta[b];
                if (data.revenue - delta + 1e-8 >= threshold()) {
                    pairs.push_back({delta, a, b});
                }
            }
        }
    }
    
    std::sort(pairs.begin(), pairs.end(), [](const PairCandidate& x, const PairCandidate& y) {
        if (x.delta != y.delta) return x.delta < y.delta;
        if (x.a != y.a) return x.a < y.a;
        return x.b < y.b;
    });

    int feasible_count = 0;
    std::size_t prefix_a = std::numeric_limits<std::size_t>::max();
    std::optional<FastRouteCheck> prefix;

    for (const auto& pair : pairs) {
        const auto threshold = std::max(global_second, result.second_gain());
        if (data.revenue - pair.delta + 1e-8 < threshold) break; 
        
        if (data.passenger) {
            FastRouteCheck candidate(in, taxi, route);
            candidate.range(0, pair.a);
            candidate.segment(data.pickup);
            candidate.segment(data.delivery);
            candidate.range(pair.a, length);
            if (candidate.feasible()) {
                consider(result, {request, taxi, pair.a, pair.a, data.revenue - pair.delta});
                if (++feasible_count >= 3) break;
            }
        } else {
            if (pair.a != prefix_a) {
                prefix_a = pair.a;
                FastRouteCheck cand(in, taxi, route);
                cand.range(0, pair.a);
                cand.segment(data.pickup);
                prefix.emplace(cand);
            }
            auto candidate = *prefix;
            if (pair.a == pair.b) {
                candidate.segment(data.delivery);
            } else {
                candidate.range(pair.a, pair.b);
                candidate.segment(data.delivery);
            }
            candidate.range(pair.b, length);
            
            if (candidate.feasible()) {
                consider(result, {request, taxi, pair.a, pair.b, data.revenue - pair.delta});
                if (++feasible_count >= 3) break;
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
        return need_second ? result.threshold(3) : result.threshold(1);
    };
    if (indexed) {
        for (const auto& [taxi, route] : state.active_routes())
            combine(result, scan_route(state, request, taxi, route, data,
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
    return insertion_options_impl(state, request, false).best();
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
    for (const auto& [taxi, route] : state.active_routes()) {
        auto& entry = row[taxi];
        const auto threshold = result.best() ? result.best()->gain :
            -std::numeric_limits<double>::infinity();
        if (entry.route_version != route.version() || threshold < entry.threshold) {
            entry.best = scan_route(state, request, taxi, route, data, threshold).best();
            entry.route_version = route.version();
            entry.threshold = threshold;
        }
        if (entry.best) consider(result, *entry.best, 1);
    }
    return result.best();
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
        options.reserve(state.active_routes().size());
        for (const auto& [taxi, route] : state.active_routes())
            options.emplace_back(taxi, scan_route(state, request, taxi, route, data));
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
