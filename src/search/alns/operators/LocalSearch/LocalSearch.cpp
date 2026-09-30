#include "search/alns/operators/LocalSearch/LocalSearch.hpp"

namespace soict::alns {
bool LocalSearch::search(SearchState& state) const {
    if (moves_.empty()) return false;
    bool any_change = false;
    std::size_t no_improve_count = 0;
    std::size_t i = 0;
    const std::size_t max_calls = 4 * moves_.size();
    std::size_t total_calls = 0;
    while (no_improve_count < moves_.size() && total_calls < max_calls) {
        if (moves_[i]->improve(state)) {
            any_change = true;
            // Move vừa chạy đã đạt local optimum trên neighbourhood của nó.
            no_improve_count = 1;
        } else {
            ++no_improve_count;
        }
        i = (i + 1) % moves_.size();
        ++total_calls;
    }
    return any_change;
}
} // namespace soict::alns
