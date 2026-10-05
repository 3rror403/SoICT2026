#include "search/alns/operators/LocalSearch/AddRequest.hpp"
#include "search/alns/operators/Repair/InsertionSearch.hpp"

#include <optional>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace soict::alns {
bool AddRequest::improve(SearchState& state) const {
    const bool indexed = bool(state.empty_taxi_index());
    if (requests_.size() != state.request_count()) {
        requests_.resize(state.request_count());
        std::iota(requests_.begin(), requests_.end(), 0);
    }
    if (indexed) cache_.prepare(state, requests_);
    bool changed = false;
    for (;;) {
        std::optional<InsertionChoice> best;
        for (const auto request : requests_) {
            if (state.location(request).taxi >= 0) continue;
            const auto move = indexed ? cache_.best(request) :
                                       fallback_cache_.best(state, request);
            if (move && move->gain > 1e-9 &&
                (!best || move->gain > best->gain)) best = move;
        }
        if (!best) break;
        if (!state.insert(best->request, best->taxi, best->pickup, best->delivery))
            throw std::logic_error("add-request descriptor disagrees with mutation");
        changed = true;
        if (indexed) cache_.refresh_taxi(state, best->taxi, requests_);
    }
    return changed;
}

} // namespace soict::alns
