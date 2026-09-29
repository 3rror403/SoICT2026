#include "search/alns/SearchState.hpp"
#include "search/alns/ALNS.hpp"
#include "search/alns/operators/Repair/InsertionSearch.hpp"
#include "search/alns/operators/Repair/GreedyInsertion.hpp"
#include "search/alns/operators/LocalSearch/PassengerRelocate.hpp"
#include "search/alns/operators/LocalSearch/ParcelRelocate.hpp"
#include "search/alns/operators/LocalSearch/AddRequest.hpp"
#include "search/alns/operators/LocalSearch/RemoveRequest.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace {
#define require(condition) do { if (!(condition)) throw std::runtime_error( \
    "ALNS test failed at line " + std::to_string(__LINE__)); } while (false)
void agree(const std::optional<double>& gain, const soict::Evaluation& full,
           double base_benefit) {
    require(static_cast<bool>(gain) == full.feasible);
    if (gain) require(std::abs(*gain - (full.benefit - base_benefit)) < 1e-7);
}
void same_options(const soict::alns::InsertionOptions& a,
                  const soict::alns::InsertionOptions& b) {
    require(static_cast<bool>(a.best) == static_cast<bool>(b.best));
    if (a.best) {
        require(a.best->request == b.best->request);
        require(a.best->taxi == b.best->taxi);
        require(a.best->pickup == b.best->pickup);
        require(a.best->delivery == b.best->delivery);
        require(std::abs(a.best->gain - b.best->gain) < 1e-8);
    }
    require(a.second_gain == b.second_gain ||
            std::abs(a.second_gain - b.second_gain) < 1e-8);
}
}

int main() {
    using namespace soict;
    Instance in;
    in.taxis = {{2}, {2}};
    in.passengers.resize(3);
    in.parcels.resize(4);
    for (auto& p : in.passengers) p = {30, {0, 1000}, 1};
    for (auto& p : in.parcels) p = {30, 1, {0, 1000}, {0, 1000}, 1};
    const auto v = in.node_count();
    in.travel_time.resize(v * v);
    in.travel_cost.resize(v * v);
    for (std::size_t u = 0; u < v; ++u)
        for (std::size_t w = 0; w < v; ++w) {
            in.travel_time[u * v + w] = u == w ? 0 : 1 + ((u * 11 + w * 7) % 13);
            in.travel_cost[u * v + w] = u == w ? 0 : 1 + ((u * 3 + w * 5) % 17);
        }
    in.validate();
    bool nonmetric = false;
    for (std::size_t u = 0; u < v; ++u)
        for (std::size_t w = 0; w < v; ++w)
            for (std::size_t mid = 0; mid < v; ++mid)
                nonmetric |= in.time(u, w) > in.time(u, mid) + in.time(mid, w);
    require(nonmetric);
    Solution initial{{{8, 2, 5, 9, 12, 13}, {10, 3, 6, 14}}};
    for (std::size_t k = 0; k < initial.routes.size(); ++k) {
        const alns::RouteCache loose(in, k, initial.routes[k]);
        for (std::size_t p = 0; p < initial.routes[k].size(); ++p) {
            const auto node = initial.routes[k][p];
            const auto latest = loose.arrival()[p] + 3;
            if (node < 2 + 3) in.passengers[node - 2].pickup.latest = latest;
            else if (node >= 2 + 6 && node < 2 + 6 + 4)
                in.parcels[node - 2 - 6].pickup.latest = latest;
            else if (node >= 2 + 6 + 4)
                in.parcels[node - 2 - 6 - 4].delivery.latest = latest;
        }
    }
    alns::SearchState state(in, initial);
    state.check();
    {
        alns::MoveCache cache;
        std::size_t scans = 0;
        const auto positive = [&]() -> std::optional<alns::InsertionChoice> {
            ++scans;
            return alns::InsertionChoice{7, 1, 1, 2, 4.0};
        };
        const auto first = cache.get(7, 1, 2, 11, 31, 10.0, false, positive);
        require(first && first->gain == 4.0);
        const auto source_changed = cache.get(7, 1, 2, 12, 31, 8.0, false, positive);
        require(source_changed && source_changed->gain == 2.0 && scans == 1);
        const auto target_changed = cache.get(7, 1, 2, 12, 32, 8.0, false, positive);
        require(target_changed && scans == 2);

        const auto no_positive = [&]() -> std::optional<alns::InsertionChoice> {
            ++scans;
            return std::nullopt;
        };
        require(!cache.get(8, 1, 2, 11, 31, 3.0, false, no_positive));
        require(!cache.get(8, 1, 2, 12, 31, 2.0, false, no_positive));
        require(scans == 3);
        require(!cache.get(8, 1, 2, 13, 31, 4.0, false, no_positive));
        require(scans == 4);
    }
    {
        auto added = state;
        alns::AddRequest{}.improve(added);
        added.check();
        for (std::size_t r = 0; r < added.request_count(); ++r) {
            if (added.location(r).taxi >= 0) continue;
            const auto best = alns::best_insertion(added, r);
            require(!best || best->gain <= 1e-9);
        }
        auto removed = state;
        alns::RemoveRequest{}.improve(removed);
        removed.check();
        for (std::size_t r = 0; r < removed.request_count(); ++r) {
            const auto gain = removed.removal_gain(r);
            require(!gain || *gain <= 1e-9);
        }
    }
    {
        Instance wide = in;
        for (auto& passenger : wide.passengers) passenger.pickup.latest = 1000;
        for (auto& parcel : wide.parcels) {
            parcel.pickup.latest = 1000;
            parcel.delivery.latest = 1000;
        }
        alns::SearchState dynamic(wide, initial); // both taxis are active
        dynamic.set_empty_taxi_index(alns::build_empty_taxi_index(dynamic,
            alns::Deadline::max()));
        alns::RepairInsertionCache cached(dynamic);
        alns::BestInsertionCache add_cache;
        const auto before = add_cache.best(dynamic, 6);
        const auto exact_before = alns::best_insertion(dynamic, 6);
        require(static_cast<bool>(before) == static_cast<bool>(exact_before));
        if (before) require(before->taxi == exact_before->taxi &&
                            before->pickup == exact_before->pickup &&
                            before->delivery == exact_before->delivery);
        same_options(cached.options(2), alns::insertion_options(dynamic, 2));
        same_options(cached.options(6), alns::insertion_options(dynamic, 6));
        const auto after = add_cache.best(dynamic, 6);
        const auto exact_after = alns::best_insertion(dynamic, 6);
        require(static_cast<bool>(after) == static_cast<bool>(exact_after));
        if (after) require(after->taxi == exact_after->taxi &&
                           after->pickup == exact_after->pickup &&
                           after->delivery == exact_after->delivery);
        const auto choice = cached.options(2).best;
        require(choice);
        require(dynamic.insert(2, choice->taxi, choice->pickup, choice->delivery));
        cached.refresh_taxi(choice->taxi);
        same_options(cached.options(6), alns::insertion_options(dynamic, 6));
        dynamic.check();
        auto restricted = alns::SearchState(wide, initial);
        restricted.set_empty_taxi_index(dynamic.empty_taxi_index());
        const std::vector<std::size_t> destroyed{2};
        std::mt19937_64 rng(1);
        alns::GreedyInsertion repair;
        repair.apply(restricted, destroyed, rng);
        require(restricted.location(6).taxi < 0);
        restricted.check();
    }

    // Compare every passenger relocation, including moves within the source
    // route, with the independent full-route evaluator on directed nonmetric data.
    for (std::size_t r = 0; r < in.passengers.size(); ++r) {
        if (state.location(r).taxi < 0) continue;
        alns::PassengerRelocationContext context(state, r);
        require(context.valid());
        const auto loc = state.location(r);
        for (std::size_t taxi = 0; taxi < in.taxis.size(); ++taxi) {
            for (std::size_t a = 0; a <= context.length(taxi); ++a) {
                auto candidate = initial;
                auto& source = candidate.routes[loc.taxi];
                source.erase(source.begin() + loc.dropoff);
                source.erase(source.begin() + loc.pickup);
                auto& destination = candidate.routes[taxi];
                destination.insert(destination.begin() + a, in.passenger_dropoff(r));
                destination.insert(destination.begin() + a, in.passenger_pickup(r));
                const auto full = candidate.evaluate(in);
                agree(context.gain(taxi, a), full, state.benefit());
                if (full.feasible) {
                    const auto objective = context.calculate_delta_fast(taxi, state.cache(taxi), a);
                    require(objective && std::abs(*objective -
                        (full.benefit - state.benefit())) < 1e-7);
                }
            }
        }
    }
    {
        auto moved = state;
        alns::PassengerRelocate move;
        move.improve(moved);
        moved.check();
        for (std::size_t r = 0; r < moved.passenger_count(); ++r) {
            if (moved.location(r).taxi < 0) continue;
            alns::PassengerRelocationContext context(moved, r);
            for (std::size_t taxi = 0; taxi < moved.taxi_count(); ++taxi)
                for (std::size_t a = 0; a <= context.length(taxi); ++a) {
                    const auto gain = context.gain(taxi, a);
                    require(!gain || *gain <= 1e-9);
                }
        }
        auto repeated = state;
        move.improve(repeated);
        require(repeated.solution().routes == moved.solution().routes);
        auto changed = state;
        require(changed.remove(0));
        auto fresh = changed;
        move.improve(changed);
        alns::PassengerRelocate{}.improve(fresh);
        require(changed.solution().routes == fresh.solution().routes);
    }
    for (std::size_t r = in.passengers.size(); r < state.request_count(); ++r) {
        if (state.location(r).taxi < 0) continue;
        alns::ParcelRelocationContext context(state, r);
        require(context.valid());
        const auto loc = state.location(r);
        for (std::size_t taxi = 0; taxi < in.taxis.size(); ++taxi) {
            for (std::size_t a = 0; a <= context.length(taxi); ++a) {
                for (std::size_t b = a; b <= context.length(taxi); ++b) {
                    auto candidate = initial;
                    auto& source = candidate.routes[loc.taxi];
                    source.erase(source.begin() + loc.dropoff);
                    source.erase(source.begin() + loc.pickup);
                    auto& destination = candidate.routes[taxi];
                    destination.insert(destination.begin() + b,
                                       in.parcel_dropoff(r - in.passengers.size()));
                    destination.insert(destination.begin() + a,
                                       in.parcel_pickup(r - in.passengers.size()));
                    const auto full = candidate.evaluate(in);
                    agree(context.gain(taxi, a, b), full, state.benefit());
                    if (full.feasible) {
                        const auto objective = context.calculate_delta_fast(taxi, state.cache(taxi), a, b);
                        require(objective && std::abs(*objective -
                            (full.benefit - state.benefit())) < 1e-7);
                    }
                }
            }
        }
    }
    {
        auto moved = state;
        alns::ParcelRelocate move;
        move.improve(moved);
        moved.check();
        for (std::size_t r = moved.passenger_count(); r < moved.request_count(); ++r) {
            if (moved.location(r).taxi < 0) continue;
            alns::ParcelRelocationContext context(moved, r);
            for (std::size_t taxi = 0; taxi < moved.taxi_count(); ++taxi)
                for (std::size_t a = 0; a <= context.length(taxi); ++a)
                    for (std::size_t b = a; b <= context.length(taxi); ++b) {
                        const auto gain = context.gain(taxi, a, b);
                        require(!gain || *gain <= 1e-9);
                    }
        }
        auto repeated = state;
        move.improve(repeated);
        require(repeated.solution().routes == moved.solution().routes);
        auto changed = state;
        require(changed.remove(0));
        auto fresh = changed;
        move.improve(changed);
        alns::ParcelRelocate{}.improve(fresh);
        require(changed.solution().routes == fresh.solution().routes);
    }

    for (std::size_t r = 0; r < state.request_count(); ++r) {
        if (state.location(r).taxi < 0) continue;
        auto candidate = initial;
        const auto loc = state.location(r);
        auto& route = candidate.routes[loc.taxi];
        route.erase(route.begin() + loc.dropoff);
        route.erase(route.begin() + loc.pickup);
        const auto full = candidate.evaluate(in);
        agree(state.removal_gain(r), full, state.benefit());
        auto changed = state;
        require(changed.remove(r) == full.feasible);
        changed.check();
    }

    for (const std::size_t request : {std::size_t{2}, std::size_t{6}}) {
        require(state.location(request).taxi < 0);
        const bool passenger = request < in.passengers.size();
        const auto pickup = passenger ? in.passenger_pickup(request) :
                                        in.parcel_pickup(request - in.passengers.size());
        const auto delivery = passenger ? in.passenger_dropoff(request) :
                                          in.parcel_dropoff(request - in.passengers.size());
        for (std::size_t k = 0; k < in.taxis.size(); ++k) {
            const auto length = initial.routes[k].size();
            for (std::size_t a = 0; a <= length; ++a)
                for (std::size_t b = a; b <= length; ++b) {
                    auto candidate = initial;
                    auto& route = candidate.routes[k];
                    route.insert(route.begin() + b, delivery);
                    route.insert(route.begin() + a, pickup);
                    const auto full = candidate.evaluate(in);
                    const auto gain = state.insertion_gain(request, k, a, b);
                    agree(gain, full, state.benefit());
                    if (gain) {
                        auto changed = state;
                        require(changed.insert(request, k, a, b));
                        changed.check();
                    }
                }
        }
        const auto options = alns::insertion_options(state, request);
        const auto best_only = alns::best_insertion(state, request);
        require(static_cast<bool>(best_only) == static_cast<bool>(options.best));
        if (best_only) {
            require(best_only->taxi == options.best->taxi);
            require(best_only->pickup == options.best->pickup);
            require(best_only->delivery == options.best->delivery);
            require(std::abs(best_only->gain - options.best->gain) < 1e-8);
        }
        std::optional<alns::InsertionChoice> exhaustive_best;
        for (std::size_t taxi = 0; taxi < in.taxis.size(); ++taxi) {
            const auto length = initial.routes[taxi].size();
            for (std::size_t a = 0; a <= length; ++a)
                for (std::size_t b = a; b <= length; ++b) {
                    const auto gain = state.insertion_gain(request, taxi, a, b);
                    if (gain && (!exhaustive_best || *gain > exhaustive_best->gain ||
                                 (*gain == exhaustive_best->gain && taxi < exhaustive_best->taxi)))
                        exhaustive_best = alns::InsertionChoice{request, taxi, a, b, *gain};
                }
        }
        require(static_cast<bool>(options.best) == static_cast<bool>(exhaustive_best));
        if (options.best) {
            require(options.best->taxi == exhaustive_best->taxi);
            require(options.best->pickup == exhaustive_best->pickup);
            require(options.best->delivery == exhaustive_best->delivery);
            require(std::abs(options.best->gain - exhaustive_best->gain) < 1e-8);
        }
    }
    alns::Config config;
    config.time_limit = 0.05;
    config.seed = 5;
    config.check = true;
    const auto solved = alns::solve(in, config);
    require(solved.solution.evaluate(in).feasible);
    require(std::abs(solved.solution.evaluate(in).benefit - solved.benefit) < 1e-7);
    config.time_limit = 1;
    config.cooling = 0.001;
    const auto annealed = alns::solve(in, config);
    require(annealed.annealing_complete);
    require(annealed.iterations == 2);
    require(annealed.solution.evaluate(in).feasible);

    // Deleting a direct passenger trip can make a later parcel pickup late
    // when a nonmetric shortcut arc replaces the removed path.
    Instance trap;
    trap.taxis = {{1}};
    trap.passengers = {{10, {0, 10}, 0}};
    trap.parcels = {{10, 1, {0, 10}, {0, 100}, 0}};
    trap.travel_time.assign(25, 0);
    trap.travel_cost.assign(25, 0);
    trap.travel_time[trap.parcel_pickup(0)] = 100; // origin 0 to pickup
    trap.validate();
    alns::SearchState trapped(trap, Solution{{{1, 2, 3, 4}}});
    require(!trapped.removal_gain(0));
    require(!trapped.remove(0));
    alns::PassengerRelocationContext trap_context(trapped, 0);
    require(trap_context.valid());
    require(trap_context.gain(0, 0));
    require(!trap_context.gain(0, 1));
    require(trapped.relocate_within_route(0, 0, 0));
    trapped.check();

    Instance parcel_trap;
    parcel_trap.taxis = {{1}};
    parcel_trap.passengers = {{10, {0, 10}, 0}};
    parcel_trap.parcels = {{10, 1, {0, 100}, {0, 100}, 0}};
    parcel_trap.travel_time.assign(25, 0);
    parcel_trap.travel_cost.assign(25, 0);
    parcel_trap.travel_time[parcel_trap.passenger_pickup(0)] = 100;
    parcel_trap.validate();
    alns::SearchState trapped_parcel(parcel_trap, Solution{{{3, 1, 2, 4}}});
    require(!trapped_parcel.removal_gain(1));
    alns::ParcelRelocationContext parcel_context(trapped_parcel, 1);
    require(parcel_context.valid());
    require(parcel_context.gain(0, 0, 2));
    require(trapped_parcel.relocate_within_route(1, 0, 2));
    trapped_parcel.check();

    // The empty-taxi index must return the same best and second-best gains as
    // scanning every taxi, even when only three of 1,000 taxis are active.
    Instance many;
    many.taxis.resize(1000, Taxi{2});
    many.passengers.resize(3, Passenger{30, {0, 1000}, 0});
    many.parcels.resize(2, Parcel{30, 1, {0, 1000}, {0, 1000}, 0});
    const auto many_nodes = many.node_count();
    many.travel_time.assign(many_nodes * many_nodes, 1);
    many.travel_cost.resize(many_nodes * many_nodes);
    for (std::size_t u = 0; u < many_nodes; ++u)
        for (std::size_t w = 0; w < many_nodes; ++w)
            many.travel_cost[u * many_nodes + w] = u == w ? 0 :
                1 + ((u * 7 + w * 3) % 11);
    many.validate();
    Solution blank;
    blank.routes.resize(1000);
    alns::SearchState blank_state(many, blank);
    const auto deadline = alns::Deadline::max();
    const auto index = alns::build_empty_taxi_index(blank_state, deadline);
    for (std::size_t r = 0; r < blank_state.request_count(); ++r)
        require(index->complete(r));
    Solution partial = blank;
    partial.routes[0] = {many.passenger_pickup(0), many.passenger_dropoff(0)};
    partial.routes[500] = {many.passenger_pickup(1), many.passenger_dropoff(1)};
    partial.routes[999] = {many.parcel_pickup(0), many.parcel_dropoff(0)};
    alns::SearchState indexed(many, partial);
    indexed.set_empty_taxi_index(index);
    {
        alns::BestInsertionCache add_cache;
        const auto before = add_cache.best(indexed, 4);
        const auto exact_before = alns::best_insertion(indexed, 4);
        require(before && exact_before && before->taxi == exact_before->taxi &&
                before->pickup == exact_before->pickup &&
                before->delivery == exact_before->delivery);
        auto changed = indexed;
        require(changed.insert(2, 250, 0, 0));
        const auto after = add_cache.best(changed, 4);
        const auto exact_after = alns::best_insertion(changed, 4);
        require(after && exact_after && after->taxi == exact_after->taxi &&
                after->pickup == exact_after->pickup &&
                after->delivery == exact_after->delivery);
    }
    {
        auto dynamic = indexed;
        alns::RepairInsertionCache cached(dynamic);
        for (const auto request : {std::size_t{2}, std::size_t{4}})
            same_options(cached.options(request), alns::insertion_options(dynamic, request));
        require(dynamic.insert(2, 250, 0, 0)); // activate an empty taxi
        cached.refresh_taxi(250);
        same_options(cached.options(4), alns::insertion_options(dynamic, 4));
        dynamic.check();
    }
    {
        auto dynamic = indexed;
        alns::RepairInsertionCache cached(dynamic);
        require(dynamic.insert(4, 500, 0, 0)); // change an active taxi
        cached.refresh_taxi(500);
        same_options(cached.options(2), alns::insertion_options(dynamic, 2));
        dynamic.check();
    }
    {
        alns::PassengerRelocationContext context(indexed, 0);
        require(context.valid());
        for (const auto taxi : {std::size_t{0}, std::size_t{1}, std::size_t{500}}) {
            for (std::size_t a = 0; a <= context.length(taxi); ++a) {
                auto candidate = partial;
                candidate.routes[0].clear();
                auto& destination = candidate.routes[taxi];
                destination.insert(destination.begin() + a, many.passenger_dropoff(0));
                destination.insert(destination.begin() + a, many.passenger_pickup(0));
                agree(context.gain(taxi, a), candidate.evaluate(many), indexed.benefit());
            }
        }
    }
    require((indexed.active_taxis() == std::vector<std::size_t>{0, 500, 999}));
    const auto compare_options = [&](std::size_t request) {
        auto plain = indexed;
        plain.set_empty_taxi_index({});
        const auto exhaustive = alns::insertion_options(plain, request);
        const auto cached = alns::insertion_options(indexed, request);
        require(static_cast<bool>(exhaustive.best) == static_cast<bool>(cached.best));
        if (cached.best) {
            require(cached.best->taxi == exhaustive.best->taxi);
            require(cached.best->pickup == exhaustive.best->pickup);
            require(cached.best->delivery == exhaustive.best->delivery);
            require(std::abs(cached.best->gain - exhaustive.best->gain) < 1e-8);
        }
        require(std::abs(cached.second_gain - exhaustive.second_gain) < 1e-8);
    };
    compare_options(2);
    compare_options(4);
    require(indexed.remove(0));
    require((indexed.active_taxis() == std::vector<std::size_t>{500, 999}));
    compare_options(0);
    require(indexed.insert(2, 250, 0, 0));
    require((indexed.active_taxis() == std::vector<std::size_t>{250, 500, 999}));
    compare_options(4);
    const auto incomplete = alns::build_empty_taxi_index(blank_state,
        std::chrono::steady_clock::now());
    require(!incomplete->complete(4));
    indexed.set_empty_taxi_index(incomplete);
    compare_options(4);
    indexed.check();
}
