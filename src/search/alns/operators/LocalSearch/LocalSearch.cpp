#include "search/alns/operators/LocalSearch/LocalSearch.hpp"

namespace soict::alns {
bool LocalSearch::search(SearchState& state) const {
    bool changed = false;
    for (int cycle = 0; cycle < 2; ++cycle) {
        bool cycle_changed = false;
        for (const auto& move : moves_) {
            cycle_changed = move->improve(state) || cycle_changed;
        }
        if (!cycle_changed) break;
        changed = true;
    }
    return changed;
}
} // namespace soict::alns
