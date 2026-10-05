#include "search/alns/operators/LocalSearch/SwapRequest.hpp"

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
    if (!use_smd_) {
        std::optional<RequestSwapChoice> best;
        for (std::size_t a = 0; a < state.request_count(); ++a) {
            if (state.location(a).taxi < 0) continue;
            for (std::size_t b = a + 1; b < state.request_count(); ++b) {
                const auto gain = request_swap_gain(state, a, b);
                if (!gain || *gain <= 1e-9) continue;
                const RequestSwapChoice choice{a, b, *gain};
                if (!best || better(choice, *best)) best = choice;
            }
        }
        return best;
    }
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
        if (!state.swap_requests(choice->first, choice->second))
            throw std::logic_error("request swap descriptor disagrees with mutation");
        changed = true;
    }
    return changed;
}
} // namespace soict::alns
