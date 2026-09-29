#include "search/alns/operators/Repair/GreedyInsertion.hpp"
#include "search/alns/operators/Repair/InsertionSearch.hpp"

namespace soict::alns {
void GreedyInsertion::apply(SearchState& state, std::span<const std::size_t> removed,
                            std::mt19937_64&) const {
    for (const auto request : removed) {
        const auto choice = best_insertion(state, request);
        if (choice && choice->gain > 1e-9)
            state.insert(request, choice->taxi, choice->pickup, choice->delivery);
    }
}
} // namespace soict::alns
