#include "search/alns/operators/LocalSearch/RemoveRequest.hpp"

#include <optional>
#include <stdexcept>
#include <vector>

namespace soict::alns {
bool RemoveRequest::improve(SearchState& state) const {
    std::vector<std::optional<double>> moves(state.request_count());
    for (std::size_t request = 0; request < state.request_count(); ++request)
        moves[request] = state.removal_gain(request);
    bool changed = false;
    for (;;) {
        std::optional<std::size_t> best;
        double best_gain = 1e-9;
        for (std::size_t request = 0; request < state.request_count(); ++request) {
            if (moves[request] && *moves[request] > best_gain) {
                best = request;
                best_gain = *moves[request];
            }
        }
        if (!best) break;
        const auto taxi = static_cast<std::size_t>(state.location(*best).taxi);
        if (!state.remove(*best))
            throw std::logic_error("remove-request descriptor disagrees with mutation");
        changed = true;
        moves[*best].reset();
        for (std::size_t request = 0; request < state.request_count(); ++request)
            if (state.location(request).taxi == static_cast<int>(taxi))
                moves[request] = state.removal_gain(request);
    }
    return changed;
}
} // namespace soict::alns
