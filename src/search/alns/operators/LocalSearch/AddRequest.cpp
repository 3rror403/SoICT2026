#include "search/alns/operators/LocalSearch/AddRequest.hpp"
#include "search/alns/operators/Repair/InsertionSearch.hpp"

#include <optional>
#include <stdexcept>
#include <vector>

namespace soict::alns {
bool AddRequest::improve(SearchState& state) const {
    std::vector<std::optional<InsertionChoice>> moves(state.request_count());
    for (std::size_t request = 0; request < state.request_count(); ++request) {
        if (state.location(request).taxi >= 0) continue;
        moves[request] = cache_.best(state, request);
    }
    bool changed = false;
    for (;;) {
        std::optional<InsertionChoice> best;
        for (const auto& move : moves)
            if (move && move->gain > 1e-9 &&
                (!best || move->gain > best->gain)) best = move;
        if (!best) break;
        if (!state.insert(best->request, best->taxi, best->pickup, best->delivery))
            throw std::logic_error("add-request descriptor disagrees with mutation");
        changed = true;
        moves[best->request].reset();
        // A changed route can improve the best position of any unserved request.
        // BestInsertionCache reuses route entries whose versions are unchanged.
        for (std::size_t request = 0; request < state.request_count(); ++request)
            if (state.location(request).taxi < 0)
                moves[request] = cache_.best(state, request);
    }
    return changed;
}

} // namespace soict::alns
