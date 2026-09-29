#include "search/alns/operators/LocalSearch/LocalSearch.hpp"

namespace soict::alns {
bool LocalSearch::search(SearchState& state) const {
    bool changed = false;
    for (const auto& move : moves_) {
        changed = move->improve(state) || changed;
    }
    return changed;
}
} // namespace soict::alns
