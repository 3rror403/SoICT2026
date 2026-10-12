#include "search/alns/ALNS.hpp"
#include "search/alns/Balance.hpp"
#include "search/alns/SearchState.hpp"
#include "search/alns/operators/Destroy/RandomRemoval.hpp"
#include "search/alns/operators/Destroy/RelatedRemoval.hpp"
#include "search/alns/operators/Destroy/WorstRemoval.hpp"
#include "search/alns/operators/Destroy/RouteRemoval.hpp"
#include "search/alns/operators/Destroy/OpportunityRemoval.hpp"
#include "search/alns/operators/LocalSearch/LocalSearch.hpp"
#include "search/alns/operators/LocalSearch/ParcelRelocate.hpp"
#include "search/alns/operators/LocalSearch/PassengerRelocate.hpp"
#include "search/alns/operators/LocalSearch/AddRequest.hpp"
#include "search/alns/operators/LocalSearch/RemoveRequest.hpp"
#include "search/alns/operators/LocalSearch/CleanCut2OptStar.hpp"
#include "search/alns/operators/LocalSearch/ReplaceRequest.hpp"
#include "search/alns/operators/LocalSearch/SwapRequest.hpp"
#include "search/alns/operators/Repair/GreedyInsertion.hpp"
#include "search/alns/operators/Repair/ProfitInsertion.hpp"
#include "search/alns/operators/Repair/RoutePoolRepair.hpp"
#include "search/alns/operators/Repair/InsertionSearch.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <numeric>
#include <random>
#include <sstream>
#include <iomanip>
#include <stdexcept>

namespace soict::alns {
Result solve(const Instance& instance, const Config& config) {
    using Clock = std::chrono::steady_clock;
    if (!config.ls_interval) throw std::invalid_argument("ls interval must be positive");
    if (config.paths_only && !config.route_pool_repair)
        throw std::invalid_argument("path generation requires route-pool repair");
    const auto start = Clock::now();
    const auto deadline = start + std::chrono::duration_cast<Clock::duration>(
                                  std::chrono::duration<double>(config.time_limit));
    std::mt19937_64 rng(config.seed);
    const double polish_reserve = config.route_pool_repair ? 1.0 : 0.0;
    const auto search_deadline = deadline - std::chrono::duration_cast<Clock::duration>(
        std::chrono::duration<double>(std::min(polish_reserve, config.time_limit * 0.05)));
    std::vector<std::unique_ptr<DestroyOperator>> destroys;
    destroys.push_back(std::make_unique<RandomRemoval>());
    destroys.push_back(std::make_unique<WorstRemoval>());
    destroys.push_back(std::make_unique<RelatedRemoval>());
    if (config.route_removal) destroys.push_back(std::make_unique<RouteRemoval>());
    if (config.opportunity_removal) destroys.push_back(std::make_unique<OpportunityRemoval>());
    std::vector<std::unique_ptr<RepairOperator>> repairs;
    repairs.push_back(std::make_unique<GreedyInsertion>());
    if (config.profit_repair) repairs.push_back(std::make_unique<ProfitInsertion>());
    struct Couple { std::size_t destroy, repair; };
    std::vector<Couple> couples;
    for (std::size_t d = 0; d < destroys.size(); ++d)
        for (std::size_t r = 0; r < repairs.size(); ++r) couples.push_back({d, r});
    Balance balance(couples.size(), instance.passengers.size() + instance.parcels.size(),
                    config.thompson);
    LocalSearch local_search;
    local_search.add_move(std::make_unique<RemoveRequest>());
    local_search.add_move(std::make_unique<AddRequest>());
    local_search.add_move(std::make_unique<PassengerRelocate>());
    local_search.add_move(std::make_unique<ParcelRelocate>());
    CleanCut2OptStar* clean_cut = nullptr;
    if (config.clean_cut_2opt_star) {
        auto move = std::make_unique<CleanCut2OptStar>();
        clean_cut = move.get();
        local_search.add_move(std::move(move));
    }
    local_search.add_move(std::make_unique<SwapRequest>());
    local_search.add_move(std::make_unique<ReplaceRequest>());

    std::ofstream trace, operator_log;
    if (!config.log_directory.empty()) {
        const std::filesystem::path root(config.log_directory);
        std::filesystem::create_directories(root / "alns");
        std::filesystem::create_directories(root / "operator");
        trace.open(root / "alns" / "progress.csv");
        operator_log.open(root / "operator" / "stats.csv");
        if (!trace || !operator_log) throw std::runtime_error("cannot create ALNS logs");
        trace << "iteration,seconds,temperature,current_benefit,best_benefit\n";
        operator_log << "destroy,repair,calls,reward_sum\n";
    }

    Solution empty;
    empty.routes.resize(instance.taxis.size());
    SearchState current(instance, std::move(empty));
    const auto index_deadline = std::min(deadline, Clock::now() +
        std::chrono::duration_cast<Clock::duration>(
            std::chrono::duration<double>(std::min(1.0, config.time_limit * 0.05))));
    current.set_empty_taxi_index(build_empty_taxi_index(current, index_deadline));
    std::unique_ptr<RoutePoolRepair> route_pool;
    bool paths_only = config.paths_only;
    if (config.route_pool_repair) {
        route_pool = std::make_unique<RoutePoolRepair>(instance, config.route_pool_limit,config.seed);
        if (config.path_generation || config.paths_only) {
            const auto generation_start = Clock::now();
            const auto generation_deadline = std::min(search_deadline, generation_start +
                std::chrono::duration_cast<Clock::duration>(std::chrono::duration<double>(
                    std::min(config.enumeration_seconds, config.time_limit * 0.2))));
            route_pool->generate(generation_deadline, config.path_prefix_limit);
            const auto initial_completed_taxis=route_pool->completed_taxis();
            const bool initial_enumeration_complete=route_pool->enumeration_complete();
            const bool sampled = config.candidate_sampling &&
                (!initial_enumeration_complete || instance.taxis.size()>48);
            if (sampled) {
                const auto beam_start = Clock::now();
                // Leave a fixed selection budget instead of abandoning a
                // large fraction of short competition runs after an easy master.
                const auto selection_reserve=std::chrono::duration_cast<Clock::duration>(
                    std::chrono::duration<double>(std::min(3.0,config.time_limit*0.05)));
                const auto beam_deadline = std::min(search_deadline-selection_reserve,beam_start+
                    std::chrono::duration_cast<Clock::duration>(std::chrono::duration<double>(
                        config.generation_seconds)));
                std::size_t phase=0;
                do {
                    const auto sample_deadline = instance.taxis.size()>48 ?
                        std::min(beam_deadline,Clock::now()+std::chrono::seconds(6)) : beam_deadline;
                    const auto initial=current.solution();
                    SearchState generated(instance,route_pool->generate_fleets(sample_deadline,current.empty_taxi_index(),&initial,
                                          config.local_search,config.clean_cut_2opt_star,config.fleet_refinement_interval,
                                          config.light_fleet_refinement,config.fleet_refinement_cycles),
                                          current.insertion_scratch_handle());
                    generated.set_empty_taxi_index(current.empty_taxi_index());current.copy_from(generated);
                    if (instance.taxis.size()>48 && Clock::now()+std::chrono::seconds(2)<beam_deadline) {
                        const auto mix_deadline=std::min(beam_deadline,Clock::now()+std::chrono::seconds(6));
                        route_pool->improve(current,mix_deadline);
                        std::cerr<<"path_feedback_phase="<<++phase<<" path_feedback_benefit="<<current.benefit()<<'\n';
                        if (Clock::now()+std::chrono::seconds(2)<beam_deadline) {
                            const auto pricing_start=Clock::now();
                            const auto price_deadline=std::min(beam_deadline,Clock::now()+std::chrono::seconds(3));
                            if (phase%2) route_pool->generate_replacements(price_deadline,current);
                            else route_pool->generate(price_deadline,config.path_prefix_limit,true);
                            std::cerr<<"path_pricing_phase="<<phase<<" path_pricing_mode="<<(phase%2?"replacement":"dfs")
                                     <<" path_pricing_seconds="<<std::chrono::duration<double>(Clock::now()-pricing_start).count();
                            if (!(phase%2)) std::cerr<<" path_pricing_prefixes="<<route_pool->generated_prefixes();
                            std::cerr<<'\n';
                        }
                    }
                } while (instance.taxis.size()>48 && Clock::now()<beam_deadline);
                paths_only = true;
            }
            if (initial_enumeration_complete) {
                paths_only = true;
                if (!sampled) route_pool->set_limit(std::min(config.route_pool_limit, config.path_pool_limit));
            }
            std::cerr << "path_prefixes=" << route_pool->total_generated_prefixes()
                      << " path_completed_taxis=" << initial_completed_taxis
                      << " path_seconds=" << std::chrono::duration<double>(Clock::now()-generation_start).count()
                      << '\n';
        }
    }
    if (!paths_only) {
        std::vector<std::size_t> initial_order(current.request_count());
        std::iota(initial_order.begin(), initial_order.end(), 0);
        std::shuffle(initial_order.begin(), initial_order.end(), rng);
        for (const auto request : initial_order) {
            if (Clock::now() >= deadline) break;
            const auto choice = best_insertion(current, request);
            if (choice && choice->gain > 1e-9)
                current.insert(request, choice->taxi, choice->pickup, choice->delivery);
        }
    }
    if (config.check) current.check();
    SearchState best = current;
    SearchState candidate = current;
    if (route_pool) route_pool->remember(current);
    double temperature = config.temperature;
    const double minimum_temperature = config.temperature * 1e-4;
    std::size_t iterations = 0;
    const auto report_progress = [&] {
        std::ostringstream line;
        line << std::fixed << std::setprecision(3)
             << '[' << std::chrono::duration<double>(Clock::now() - start).count()
             << " s] Iteration " << iterations
             << std::setprecision(6) << ", Temperature: " << temperature
             << std::setprecision(0)
             << ", Current obj: " << current.benefit()
             << ", Best obj: " << best.benefit() << '\n';
        std::cerr << line.str();
    };
    report_progress();
    if (trace) trace << "0,0," << temperature << ',' << current.benefit() << ','
                     << best.benefit() << '\n';

    while (!paths_only && temperature > minimum_temperature) {
        const auto iteration_start = Clock::now();
        if (iteration_start >= search_deadline) break;
        const auto selection = balance.select(rng, current.served_count());
        const auto couple = couples[selection.couple];
        candidate.copy_from(current);
        const auto removed = destroys[couple.destroy]->apply(
            candidate, selection.removal_count, rng);
        if (config.all_unserved_repair) {
            std::vector<std::size_t> pool;
            for (std::size_t r = 0; r < candidate.request_count(); ++r)
                if (candidate.location(r).taxi < 0) pool.push_back(r);
            std::shuffle(pool.begin(), pool.end(), rng);
            repairs[couple.repair]->apply(candidate, pool, rng);
        } else repairs[couple.repair]->apply(candidate, removed, rng);
        if (config.local_search && iterations % config.ls_interval == 0)
            local_search.search(candidate);
        if (config.check) candidate.check();
        if (route_pool) route_pool->remember(candidate);
        const double delta = candidate.benefit() - current.benefit();
        const double reward = std::max(0.0, delta / std::max(1.0, std::abs(current.benefit())));
        bool accept = delta >= 0;
        if (!accept) {
            std::uniform_real_distribution<double> uniform(0, 1);
            accept = uniform(rng) < std::exp(delta / temperature);
        }
        if (accept) {
            current.swap(candidate);
            if (current.benefit() > best.benefit() + 1e-9) {
                best.copy_from(current);
                std::ostringstream line;
                line << std::fixed << std::setprecision(3)
                     << '[' << std::chrono::duration<double>(Clock::now() - start).count()
                     << " s] Iteration " << iterations + 1
                     << std::setprecision(0)
                     << ": New best objective = " << best.benefit() << '\n';
                std::cerr << line.str();
            }
        }
        balance.update(selection, reward);
        ++iterations;
        temperature = std::max(minimum_temperature, temperature * config.cooling);
        report_progress();
        if (iterations % 1000 == 0) {
            const double seconds = std::chrono::duration<double>(Clock::now() - start).count();
            if (trace) trace << iterations << ',' << seconds << ',' << temperature << ','
                             << current.benefit() << ',' << best.benefit() << '\n';
        }
    }
    if (route_pool) {
        const auto before = best.benefit();
        const auto pool_start = Clock::now();
        route_pool->improve(best, deadline);
        std::cerr << "route_pool_columns=" << route_pool->columns()
                  << " route_pool_nodes=" << route_pool->nodes()
                  << " route_pool_timeout=" << route_pool->timed_out()
                  << " route_pool_complete=" << route_pool->master_complete()
                  << " route_pool_seconds=" << std::chrono::duration<double>(Clock::now()-pool_start).count()
                  << " route_pool_gain=" << best.benefit() - before << '\n';
        // Recombination can expose insertions and exchanges absent from the
        // retained bank. Polish only complete moves within the remaining budget.
        if (paths_only && config.local_search && Clock::now()+std::chrono::milliseconds(500)<deadline) {
            const auto polish_start=Clock::now();
            const auto before_polish=best.benefit();
            local_search.search(best,deadline);
            if (best.benefit()>before_polish+1e-9) route_pool->remember(best);
            std::cerr<<"path_final_polish_seconds="<<std::chrono::duration<double>(Clock::now()-polish_start).count()
                     <<" path_final_polish_gain="<<best.benefit()-before_polish<<'\n';
        }
        if (!config.log_directory.empty()) {
            std::ofstream bank(std::filesystem::path(config.log_directory) / "route-pool.csv");
            if (!bank) throw std::runtime_error("cannot create route pool log");
            route_pool->write(bank);
        }
    }
    if (config.check) best.check();
    if (trace) trace << iterations << ','
                     << std::chrono::duration<double>(Clock::now() - start).count() << ','
                     << temperature << ',' << current.benefit() << ','
                     << best.benefit() << '\n';
    for (std::size_t c = 0; c < couples.size() && operator_log; ++c)
        operator_log << destroys[couples[c].destroy]->name() << ','
                     << repairs[couples[c].repair]->name() << ','
                     << balance.calls()[c] << ',' << balance.rewards()[c] << '\n';
    Result result;
    result.solution = best.solution();
    result.benefit = best.benefit();
    result.iterations = iterations;
    result.seconds = std::chrono::duration<double>(Clock::now() - start).count();
    result.annealing_complete = temperature <= minimum_temperature;
    result.paths_only = paths_only && !route_pool->timed_out();
    if (clean_cut)
        std::cerr << "clean_cut_calls=" << clean_cut->calls()
                  << " clean_cut_moves=" << clean_cut->applied_moves()
                  << " clean_cut_gain=" << clean_cut->total_gain() << '\n';
    return result;
}
} // namespace soict::alns
