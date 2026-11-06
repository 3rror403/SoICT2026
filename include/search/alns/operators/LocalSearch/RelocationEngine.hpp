#pragma once

#include "search/alns/SearchState.hpp"
#include "search/alns/operators/LocalSearch/RelocationPool.hpp"
#include <stdexcept>

namespace soict::alns {

// Shared request/taxi traversal for passenger and parcel relocation. The
// context and best_on_taxi callback retain the event-specific feasibility
// rules; this engine owns only cache reuse, affected-taxi rescans, and move
// application order.
template<class Context, bool Passenger, class BestOnTaxi>
bool improve_relocations(SearchState& state, std::size_t first_request,
                         std::size_t end_request,
                         BestOnTaxi best_on_taxi) {
    RelocationPool pool;
    const auto scan_request = [&](std::size_t request, bool all,
                                  std::size_t first, std::size_t second) {
        if (state.location(request).taxi < 0) return;
        Context context(state, request);
        const auto source = static_cast<std::size_t>(state.location(request).taxi);
        if (!context.valid()) return;
        const auto source_version = state.cache(source).version();
        const auto& index = state.empty_taxi_index();
        const bool indexed = index && index->ensure(request);
        if (indexed) {
            if (all) {
                static const RouteCache empty_route;
                for (const auto& choice : index->choices(request)) {
                    if (state.is_active(choice.taxi)) continue;
                    const auto gain = [&] {
                        if constexpr (Passenger) return context.gain(choice.taxi, empty_route, 0);
                        else return context.gain(choice.taxi, empty_route, 0, 0);
                    }();
                    if (gain) pool.add(InsertionChoice{request, choice.taxi, 0, 0, *gain}, source);
                    break;
                }
            }
            for (const auto taxi : state.active_taxis())
                if (all || taxi == first || taxi == second)
                    pool.add(best_on_taxi(state, context, request, taxi,
                                         state.cache(taxi), source_version), source);
        } else {
            for (std::size_t taxi = 0; taxi < state.taxi_count(); ++taxi)
                if (all || taxi == first || taxi == second)
                    pool.add(best_on_taxi(state, context, request, taxi,
                                         state.cache(taxi), source_version), source);
        }
    };
    const auto scan_all = [&] {
        pool.clear();
        for (std::size_t request = first_request; request < end_request; ++request)
            scan_request(request, true, 0, 0);
    };
    scan_all();
    bool changed = false;
    while (const auto descriptor = pool.best()) {
        const auto choice = descriptor->choice;
        const auto source = descriptor->source;
        const auto target = choice.taxi;
        const bool source_active_before = state.is_active(source);
        const bool target_active_before = state.is_active(target);
        const bool same = source == target;
        const bool applied = same ? state.relocate_within_route(
            choice.request, choice.pickup, choice.delivery) :
            (state.remove(choice.request) && state.insert(choice.request,
                target, choice.pickup, choice.delivery));
        if (!applied) throw std::logic_error(Passenger ?
            "passenger relocation descriptor disagrees with mutation" :
            "parcel relocation descriptor disagrees with mutation");
        changed = true;
        if (state.is_active(source) != source_active_before ||
            state.is_active(target) != target_active_before) {
            scan_all();
            continue;
        }
        pool.erase_affected(source, target);
        for (std::size_t request = first_request; request < end_request; ++request) {
            if (state.location(request).taxi < 0) continue;
            const auto origin = static_cast<std::size_t>(state.location(request).taxi);
            scan_request(request, origin == source || origin == target, source, target);
        }
    }
    return changed;
}

} // namespace soict::alns
