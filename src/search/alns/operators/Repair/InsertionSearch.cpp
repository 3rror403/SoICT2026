#include "search/alns/operators/Repair/InsertionSearch.hpp"
#include "search/alns/FastRouteCheck.hpp"
#include "search/alns/InsertionScratch.hpp"

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

namespace {
InsertionOptions insertion_options_impl(const SearchState& state, std::size_t request,
                                         bool need_second) {
    InsertionOptions result;
    if (state.location(request).taxi >= 0) return result;
    const bool indexed = add_empty_taxis(result, state, request);
    const auto data = request_segments(state, request);
    auto& scratch = state.insertion_scratch();
    const auto threshold = [&] {
        return need_second ? result.second_gain :
            (result.best ? result.best->gain : -std::numeric_limits<double>::infinity());
    };
    if (indexed) {
        for (const auto taxi : state.active_taxis())
            combine(result, scan_route(state, request, taxi, state.cache(taxi), data,
                                       scratch, threshold(), !need_second));
    } else {
        for (std::size_t taxi = 0; taxi < state.taxi_count(); ++taxi)
            combine(result, scan_route(state, request, taxi, state.cache(taxi), data,
                                       scratch, threshold(), !need_second));
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
    const auto& data = state.empty_taxi_index()->request_segments(request);
    auto& scratch = state.insertion_scratch();
    for (const auto taxi : state.active_taxis()) {
        const auto& route = state.cache(taxi);
        auto& entry = row[taxi];
        const auto threshold = result.best ? result.best->gain :
            -std::numeric_limits<double>::infinity();
        if (entry.route_version != route.version() || threshold < entry.threshold) {
            entry.best = scan_route(state, request, taxi, route, data, scratch, threshold, true).best;
            entry.route_version = route.version();
            entry.threshold = threshold;
        }
        if (entry.best) consider(result, *entry.best);
    }
    return result.best;
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
    if (!index) throw std::logic_error("positive insertion cache requires empty taxi index");
    if (index_ != index || taxis_ != state.taxi_count() ||
        requests_ != state.request_count()) {
        index_ = index;
        taxis_ = state.taxi_count();
        requests_ = state.request_count();
        if (taxis_ == std::numeric_limits<std::size_t>::max() ||
            requests_ > std::numeric_limits<std::size_t>::max() / (taxis_ + 1))
            throw std::length_error("positive insertion cache size overflow");
        entries_.assign(requests_ * (taxis_ + 1), Entry{});
    }
}

const std::optional<InsertionChoice>& PositiveInsertionCache::best(const SearchState& state,
    std::size_t request) {
    initialize(state);
    if (state.location(request).taxi >= 0) {
        entries_.at(requests_ * taxis_ + request).choice.reset();
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
    std::span<const std::size_t> requests) {
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
    std::size_t taxi, std::span<const std::size_t> requests) {
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

bool RepairInsertionCache::enabled_for(const SearchState& state) {
    return state.empty_taxi_index() && state.request_count() - state.served_count() >= 4;
}

RepairInsertionCache::RepairInsertionCache(const SearchState& state)
    : state_(state), route_options_(state.request_count()) {
    requests_.resize(state.request_count());
    std::iota(requests_.begin(), requests_.end(), 0);
    if (!state.empty_taxi_index()) return;
    for (const auto request : requests_) {
        if (state.location(request).taxi >= 0) continue;
        const auto data = request_segments(state, request);
        auto& options = route_options_[request];
        options.reserve(state.active_taxis().size());
        for (const auto taxi : state.active_taxis())
            options.emplace_back(taxi, scan_route(state, request, taxi,
                                                 state.cache(taxi), data, state.insertion_scratch()));
    }
}

RepairInsertionCache::RepairInsertionCache(const SearchState& state,
    std::span<const std::size_t> requests)
    : state_(state), route_options_(state.request_count()) {
    std::vector<bool> included(state.request_count(), false);
    for (const auto request : requests) {
        if (included.at(request) || state.location(request).taxi >= 0) continue;
        included[request] = true;
        requests_.push_back(request);
        if (!state.empty_taxi_index()) continue;
        const auto data = request_segments(state, request);
        for (const auto taxi : state.active_taxis())
            route_options_[request].emplace_back(taxi, scan_route(state, request,
                taxi, state.cache(taxi), data, state.insertion_scratch()));
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
    for (const auto request : requests_) {
        if (state_.location(request).taxi >= 0) continue;
        auto& options = route_options_[request];
        const auto found = std::lower_bound(options.begin(), options.end(), taxi,
            [](const TaxiOptions& entry, std::size_t id) { return entry.first < id; });
        const auto data = request_segments(state_, request);
        auto updated = scan_route(state_, request, taxi, route, data, state_.insertion_scratch());
        if (found == options.end() || found->first != taxi)
            options.insert(found, {taxi, std::move(updated)});
        else found->second = std::move(updated);
    }
}
} // namespace soict::alns
