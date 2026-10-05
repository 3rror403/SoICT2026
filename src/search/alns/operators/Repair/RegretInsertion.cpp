#include "search/alns/operators/Repair/RegretInsertion.hpp"
#include "search/alns/operators/Repair/InsertionSearch.hpp"

#include <cmath>

namespace soict::alns {
void RegretInsertion::apply(SearchState& state, std::span<const std::size_t> removed,
                            std::mt19937_64&) const {
    std::vector<bool> pending(state.request_count(), false);
    for (const auto request : removed) pending.at(request) = true;
    // A repair mutates only one taxi per insertion. Cache only its candidate
    // pool and rescan the changed taxi, keeping exactly the same ordering.
    RepairInsertionCache cache(state, removed);
    while (true) {
        std::optional<InsertionChoice> choice;
        double best_regret = -1;
        for (const auto request : removed) {
            if (!pending[request]) continue;
            const auto options = cache.options(request);
            if (!options.best || options.best->gain <= 1e-9) continue;
            const double regret = std::isfinite(options.second_gain) ?
                                  options.best->gain - options.second_gain :
                                  options.best->gain;
            if (!choice || regret > best_regret ||
                (regret == best_regret && options.best->gain > choice->gain)) {
                choice = options.best;
                best_regret = regret;
            }
        }
        if (!choice || !state.insert(choice->request, choice->taxi,
                                      choice->pickup, choice->delivery)) return;
        pending[choice->request] = false;
        cache.refresh_taxi(choice->taxi);
    }
}
} // namespace soict::alns
