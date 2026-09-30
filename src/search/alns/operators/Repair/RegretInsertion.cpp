#include "search/alns/operators/Repair/RegretInsertion.hpp"
#include "search/alns/operators/Repair/InsertionSearch.hpp"

#include <cmath>
#include <limits>
#include <vector>

namespace soict::alns {

// ─── Shared helpers ────────────────────────────────────────────────────────

namespace {

// Select the pending request with the highest regret and insert it.
// Returns false when no insertion is possible.
using RegretFn = double (*)(const InsertionOptions&);

void regret_apply(SearchState& state,
                  std::span<const std::size_t> removed,
                  RegretFn regret_of) {
    std::vector<bool> pending(state.request_count(), false);
    for (const auto request : removed) pending.at(request) = true;
    while (true) {
        std::optional<InsertionChoice> choice;
        double best_regret = -1;
        for (const auto request : removed) {
            if (!pending[request]) continue;
            const auto options = insertion_options(state, request);
            if (!options.best || options.best->gain <= 1e-9) continue;
            const double regret = regret_of(options);
            // Tie-break: prefer higher best->gain when regret is equal.
            if (!choice || regret > best_regret ||
                (regret == best_regret && options.best->gain > choice->gain)) {
                choice = options.best;
                best_regret = regret;
            }
        }
        if (!choice || !state.insert(choice->request, choice->taxi,
                                      choice->pickup, choice->delivery)) return;
        pending[choice->request] = false;
    }
}

} // namespace

// ─── V0: original behaviour ────────────────────────────────────────────────
// regret = best - second  (if second is finite)
//        = best           (if only 1 feasible position, second = -inf)
//
// Note: when second is finite but negative (feasible insertion with cost > revenue),
// regret can exceed best->gain. This differs from V1.
void RegretInsertionV0::apply(SearchState& state,
                               std::span<const std::size_t> removed,
                               std::mt19937_64&) const {
    regret_apply(state, removed, [](const InsertionOptions& opts) -> double {
        return std::isfinite(opts.second_gain)
            ? opts.best->gain - opts.second_gain
            : opts.best->gain;
    });
}

// ─── V1: best - max(second, 0) ─────────────────────────────────────────────
// Treats "not serving this request" (gain = 0) as the baseline alternative.
// When second < 0 (feasible but costly position), ignores it and uses 0.
// Matches V0 for the 1-option case. Differs when second is finite and negative.
void RegretInsertionV1::apply(SearchState& state,
                               std::span<const std::size_t> removed,
                               std::mt19937_64&) const {
    regret_apply(state, removed, [](const InsertionOptions& opts) -> double {
        return opts.best->gain - std::max(opts.second_gain, 0.0);
    });
}

// ─── V2: +inf priority for 1-option requests ──────────────────────────────
// When a request has only one feasible position, it is absolutely prioritised.
// Among such requests, tie-break by best->gain (handled by regret_apply
// tie-break logic since all receive +inf regret).
// For 2+ option requests: regret = best - second (same as V0).
void RegretInsertionV2::apply(SearchState& state,
                               std::span<const std::size_t> removed,
                               std::mt19937_64&) const {
    regret_apply(state, removed, [](const InsertionOptions& opts) -> double {
        return std::isfinite(opts.second_gain)
            ? opts.best->gain - opts.second_gain
            : std::numeric_limits<double>::infinity();
    });
}

} // namespace soict::alns
