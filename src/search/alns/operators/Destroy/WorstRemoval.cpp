#include "search/alns/operators/Destroy/WorstRemoval.hpp"

#include <limits>

namespace soict::alns {
std::vector<std::size_t> WorstRemoval::apply(SearchState& state, std::size_t count,
                                std::mt19937_64&) const {
    std::vector<std::size_t> removed;
    removed.reserve(count);
    while (removed.size() < count) {
        std::size_t choice = state.request_count();
        double best_gain = -std::numeric_limits<double>::infinity();
        for (std::size_t r = 0; r < state.request_count(); ++r) {
            const auto gain = state.removal_gain(r);
            if (gain && *gain > best_gain) { choice = r; best_gain = *gain; }
        }
        if (choice == state.request_count() || !state.remove(choice)) break;
        removed.push_back(choice);
    }
    return removed;
}
} // namespace soict::alns
