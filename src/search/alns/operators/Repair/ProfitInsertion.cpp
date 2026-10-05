#include "search/alns/operators/Repair/ProfitInsertion.hpp"
#include <stdexcept>

namespace soict::alns {
void ProfitInsertion::apply(SearchState& state, std::span<const std::size_t> pool,
                            std::mt19937_64&) const {
    const bool indexed = bool(state.empty_taxi_index());
    if (indexed) positive_cache_.prepare(state, pool);
    for (;;) {
        std::optional<InsertionChoice> choice;
        double highest = 0;
        for (const auto request : pool) {
            if (state.location(request).taxi >= 0) continue;
            const auto option = indexed ? positive_cache_.best(request) :
                                          fallback_cache_.best(state, request);
            if (option && option->gain > 1e-9 && option->gain > highest) {
                choice = option;
                highest = option->gain;
            }
        }
        if (!choice) return;
        if (!state.insert(choice->request, choice->taxi, choice->pickup, choice->delivery))
            throw std::logic_error("profit repair descriptor disagrees with insertion");
        if (indexed) positive_cache_.refresh_taxi(state, choice->taxi, pool);
    }
}
} // namespace soict::alns
