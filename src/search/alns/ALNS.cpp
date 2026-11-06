#include "search/alns/ALNS.hpp"
#include "search/alns/RouteGeneration.hpp"
#include "search/alns/operators/LocalSearch/LocalSearch.hpp"
#include "search/alns/operators/LocalSearch/ParcelRelocate.hpp"
#include "search/alns/operators/LocalSearch/PassengerRelocate.hpp"
#include "search/alns/operators/LocalSearch/AddRequest.hpp"
#include "search/alns/operators/LocalSearch/RemoveRequest.hpp"
#include "search/alns/operators/LocalSearch/CleanCut2OptStar.hpp"
#include "search/alns/operators/LocalSearch/ReplaceRequest.hpp"
#include "search/alns/operators/LocalSearch/SwapRequest.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#ifndef SOICT_SUBMISSION
#include <filesystem>
#include <fstream>
#include <stdexcept>
#endif

namespace soict::alns {
Result solve(const Instance& instance, const Config& config) {
    using Clock = std::chrono::steady_clock;
    const auto start = Clock::now();
    const auto deadline = start + std::chrono::duration_cast<Clock::duration>(
        std::chrono::duration<double>(config.time_limit));
    const auto search_deadline = deadline - std::chrono::duration_cast<Clock::duration>(
        std::chrono::duration<double>(std::min(1.0, config.time_limit * 0.05)));
    LocalSearch local_search;
    local_search.add_move(std::make_unique<RemoveRequest>());
    local_search.add_move(std::make_unique<AddRequest>());
    local_search.add_move(std::make_unique<PassengerRelocate>());
    local_search.add_move(std::make_unique<ParcelRelocate>());
#ifndef SOICT_SUBMISSION
    CleanCut2OptStar* clean_cut = nullptr;
#endif
    if (config.clean_cut_2opt_star) {
        auto move = std::make_unique<CleanCut2OptStar>();
#ifndef SOICT_SUBMISSION
        clean_cut = move.get();
#endif
        local_search.add_move(std::move(move));
    }
    local_search.add_move(std::make_unique<SwapRequest>());
    local_search.add_move(std::make_unique<ReplaceRequest>());

#ifndef SOICT_SUBMISSION
    std::ofstream trace;
    if (!config.log_directory.empty()) {
        const std::filesystem::path root(config.log_directory);
        std::filesystem::create_directories(root / "alns");
        trace.open(root / "alns" / "progress.csv");
        if (!trace) throw std::runtime_error("cannot create solver log");
        trace << "iteration,seconds,temperature,current_benefit,best_benefit\n";
    }
#endif
    Solution empty;
    empty.routes.resize(instance.taxis.size());
    SearchState current(instance, std::move(empty));
    const auto index_deadline = std::min(deadline, Clock::now() +
        std::chrono::duration_cast<Clock::duration>(
            std::chrono::duration<double>(std::min(1.0, config.time_limit * 0.05))));
    current.set_empty_taxi_index(build_empty_taxi_index(current, index_deadline));
    RoutePoolRepair route_pool(instance, config.route_pool_limit, config.seed);
    generate_routes(current, route_pool, config, search_deadline);
    if (config.check) current.check();
    SearchState best = current;
    route_pool.remember(current);
    const auto before = best.benefit();
    const auto pool_start = Clock::now();
    route_pool.improve(best, deadline);
    std::cerr << "route_pool_columns=" << route_pool.columns()
              << " route_pool_nodes=" << route_pool.nodes()
              << " route_pool_timeout=" << route_pool.timed_out()
              << " route_pool_complete=" << route_pool.master_complete()
              << " route_pool_seconds=" << std::chrono::duration<double>(Clock::now()-pool_start).count()
              << " route_pool_gain=" << best.benefit() - before << '\n';
    // Keep the same final complete-move polishing and remaining-time gate.
    if (config.local_search && Clock::now()+std::chrono::milliseconds(500)<deadline) {
        const auto polish_start = Clock::now();
        const auto before_polish = best.benefit();
        local_search.search(best, deadline);
        if (best.benefit()>before_polish+1e-9) route_pool.remember(best);
        std::cerr << "path_final_polish_seconds=" << std::chrono::duration<double>(Clock::now()-polish_start).count()
                  << " path_final_polish_gain=" << best.benefit()-before_polish << '\n';
    }
#ifndef SOICT_SUBMISSION
    if (!config.log_directory.empty()) {
        std::ofstream bank(std::filesystem::path(config.log_directory) / "route-pool.csv");
        if (!bank) throw std::runtime_error("cannot create route pool log");
        route_pool.write(bank);
    }
#endif
    if (config.check) best.check();
    Result result;
    result.solution = best.solution();
#ifndef SOICT_SUBMISSION
    result.benefit = best.benefit();
    result.seconds = std::chrono::duration<double>(Clock::now() - start).count();
    result.timed_out = route_pool.timed_out();
    if (trace) trace << "0," << result.seconds << ",0," << current.benefit() << ',' << best.benefit() << '\n';
    if (clean_cut)
        std::cerr << "clean_cut_calls=" << clean_cut->calls()
                  << " clean_cut_moves=" << clean_cut->applied_moves()
                  << " clean_cut_gain=" << clean_cut->total_gain() << '\n';
#endif
    return result;
}
} // namespace soict::alns
