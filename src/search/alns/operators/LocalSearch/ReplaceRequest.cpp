#include "search/alns/operators/LocalSearch/ReplaceRequest.hpp"
#include "search/alns/MoveCost.hpp"

#include <algorithm>
#include <stdexcept>
#include <tuple>

namespace soict::alns {
std::optional<ReplacementChoice> ReplaceRequest::best_choice(const SearchState& state) const {
    if (!use_smd_) return best_choice_reference(state);
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

std::optional<ReplacementChoice> ReplaceRequest::best_choice_reference(const SearchState& state) const {
    std::vector<std::size_t> unserved;
    for (std::size_t r = 0; r < state.request_count(); ++r)
        if (state.location(r).taxi < 0) unserved.push_back(r);
    if (unserved.empty()) return std::nullopt;
    rows_.resize(state.request_count());
    std::optional<ReplacementChoice> best;
    double best_gain = 1e-9;
    std::vector<std::size_t> nodes;
    RouteCache reduced;
    for (std::size_t outgoing = 0; outgoing < state.request_count(); ++outgoing) {
        const auto loc = state.location(outgoing);
        if (loc.taxi < 0) continue;
        const auto taxi = static_cast<std::size_t>(loc.taxi);
        const auto& original = state.cache(taxi);
        auto& row = rows_[outgoing];
        if (row.route_version != original.version()) {
            row.route_version = original.version();
            row.entries.clear();
            row.entries.resize(state.request_count());
        }
        const double removal_gain = removal_savings_cost(state.instance(), taxi,
            original.nodes(), loc.pickup, loc.dropoff) - state.revenue(outgoing);
        bool prepared = false;
        for (const auto incoming : unserved) {
            auto& entry = row.entries[incoming];
            // A cached positive choice is the exact best position. A scan
            // finding no choice is reusable only at an equal/higher threshold.
            if (!entry.valid || (!entry.choice && best_gain < entry.threshold)) {
                if (!prepared) {
                    nodes = original.nodes();
                    nodes.erase(nodes.begin() + loc.dropoff);
                    nodes.erase(nodes.begin() + loc.pickup);
                    reduced.rebuild(state.instance(), taxi, nodes);
                    prepared = true;
                }
                entry.valid = true;
                entry.threshold = best_gain;
                entry.choice.reset();
                const auto insertion = best_insertion_on_route(state, incoming, taxi,
                    reduced, best_gain - removal_gain);
                if (insertion && insertion->gain + removal_gain > best_gain)
                    entry.choice = ReplacementChoice{outgoing, *insertion,
                                                       insertion->gain + removal_gain};
            }
            if (entry.choice && entry.choice->gain > best_gain) {
                best = entry.choice;
                best_gain = best->gain;
            }
        }
    }
    return best;
}

bool ReplaceRequest::improve(SearchState& state) const {
    bool changed = false;
    while (const auto choice = best_choice(state)) {
        if (!state.replace_request(choice->outgoing, choice->insertion.request,
                                   choice->insertion.pickup, choice->insertion.delivery))
            throw std::logic_error("replacement descriptor disagrees with mutation");
        changed = true;
    }
    return changed;
}
} // namespace soict::alns
