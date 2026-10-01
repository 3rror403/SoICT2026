#include "search/alns/operators/LocalSearch/LocalSearch.hpp"

namespace soict::alns {
bool LocalSearch::search(SearchState& state, const std::vector<std::unique_ptr<Move>>& moves) const {
    if (moves.empty()) return false;
    bool any_change = false;
    std::size_t no_improve_count = 0;
    std::size_t i = 0;
    const std::size_t max_calls = 4 * moves.size();
    std::size_t total_calls = 0;
    while (no_improve_count < moves.size() && total_calls < max_calls) {
        if (moves[i]->improve(state)) {
            any_change = true;
            no_improve_count = 1;
        } else {
            ++no_improve_count;
        }
        i = (i + 1) % moves.size();
        ++total_calls;
    }
    return any_change;
}
} // namespace soict::alns
