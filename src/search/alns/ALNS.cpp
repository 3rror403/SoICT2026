#include "search/alns/ALNS.hpp"
#include "search/alns/Balance.hpp"
#include "search/alns/SearchState.hpp"
#include "search/alns/operators/Destroy/RandomRemoval.hpp"
#include "search/alns/operators/Destroy/RelatedRemoval.hpp"
#include "search/alns/operators/Destroy/RouteRemoval.hpp"
#include "search/alns/operators/Destroy/WorstRemoval.hpp"
#include "search/alns/operators/Destroy/WorstMarginalBenefitRemoval.hpp"
#include "search/alns/operators/Destroy/DelayRemoval.hpp"
#include "search/alns/operators/Destroy/TimeBucketRemoval.hpp"
#include "search/alns/operators/LocalSearch/LocalSearch.hpp"
#include "search/alns/operators/LocalSearch/ParcelRelocate.hpp"
#include "search/alns/operators/LocalSearch/PassengerRelocate.hpp"
#include "search/alns/operators/LocalSearch/AddRequest.hpp"
#include "search/alns/operators/LocalSearch/RemoveRequest.hpp"
#include "search/alns/operators/LocalSearch/ReplaceRequest.hpp"
#include "search/alns/operators/LocalSearch/SwapRequest.hpp"
#include "search/alns/operators/LocalSearch/CleanCut2OptStar.hpp"
#include "search/alns/operators/Repair/GreedyInsertion.hpp"
#include "search/alns/operators/Repair/RegretInsertion.hpp"
#include "search/alns/operators/Repair/SharedGapPairRepair.hpp"
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
#include <stdexcept>

namespace soict::alns {
Result solve(const Instance& instance, const Config& config) {
    using Clock = std::chrono::steady_clock;
    if (!config.ls_interval) throw std::invalid_argument("ls interval must be positive");
    const auto start = Clock::now();
    const auto deadline = start + std::chrono::duration_cast<Clock::duration>(
                                  std::chrono::duration<double>(config.time_limit));

    // ── Phase 0d: progress ∈ [0,1] controls both cooling and loop termination.
    // Iteration mode: progress = iteration / max_iterations (deterministic).
    // Time mode:      progress = elapsed / time_limit        (default).
    const bool iteration_mode = config.max_iterations.has_value();
    const std::size_t max_iter = iteration_mode ? *config.max_iterations : 0;
    const auto get_progress = [&](std::size_t iter) -> double {
        if (iteration_mode)
            return static_cast<double>(iter) / static_cast<double>(max_iter);
        const double elapsed = std::chrono::duration<double>(Clock::now() - start).count();
        return elapsed / config.time_limit;
    };

    std::mt19937_64 rng(config.seed);
    std::vector<std::unique_ptr<DestroyOperator>> destroys;
    destroys.push_back(std::make_unique<RandomRemoval>());
    destroys.push_back(std::make_unique<WorstRemoval>());
    destroys.push_back(std::make_unique<WorstMarginalBenefitRemoval>());
    destroys.push_back(std::make_unique<DelayRemoval>());
    destroys.push_back(std::make_unique<TimeBucketRemoval>());
    destroys.push_back(std::make_unique<RelatedRemoval>());
    destroys.push_back(std::make_unique<RouteRemoval>());
    std::vector<std::unique_ptr<RepairOperator>> repairs;
    repairs.push_back(std::make_unique<GreedyInsertion>());
    repairs.push_back(std::make_unique<SharedGapPairRepair>(instance));
    // repairs.push_back(std::make_unique<RegretInsertionV0>()); // Weak standard regret
    repairs.push_back(std::make_unique<RegretInsertionV1>());
    repairs.push_back(std::make_unique<RegretInsertionV2>());
    // repairs.push_back(std::make_unique<GlobalRegret3>());     // Weak global regret
    // repairs.push_back(std::make_unique<RouteRegret2>());      // Suboptimal route regret
    repairs.push_back(std::make_unique<RouteRegret3>());
    struct Couple { std::size_t destroy, repair; };
    std::vector<Couple> couples;
    for (std::size_t d = 0; d < destroys.size(); ++d)
        for (std::size_t r = 0; r < repairs.size(); ++r) couples.push_back({d, r});
    Balance balance(couples.size(), instance.passengers.size() + instance.parcels.size(),
                    config.thompson);
    LocalSearch local_search;
    local_search.add_tier1(std::make_unique<RemoveRequest>());
    local_search.add_tier1(std::make_unique<AddRequest>());
    local_search.add_tier1(std::make_unique<PassengerRelocate>());
    local_search.add_tier1(std::make_unique<ParcelRelocate>());
    local_search.add_tier1(std::make_unique<ReplaceRequest>());
    
    local_search.add_tier2(std::make_unique<SwapRequest>());
    local_search.add_tier2(std::make_unique<CleanCut2OptStar>());

    std::ofstream trace, operator_log;
    if (!config.log_directory.empty()) {
        const std::filesystem::path root(config.log_directory);
        std::filesystem::create_directories(root / "alns");
        std::filesystem::create_directories(root / "operator");
        trace.open(root / "alns" / "progress.csv");
        operator_log.open(root / "operator" / "stats.csv");
        if (!trace || !operator_log) throw std::runtime_error("cannot create ALNS logs");
        // Phase 0c: added seconds and progress columns for profiling.
        trace << "iteration,seconds,progress,temperature,current_benefit,best_benefit\n";
        operator_log << "destroy,repair,calls,reward_sum\n";
    }

    Solution empty;
    empty.routes.resize(instance.taxis.size());
    SearchState current(instance, std::move(empty));
    const auto index_deadline = std::min(deadline, Clock::now() +
        std::chrono::duration_cast<Clock::duration>(
            std::chrono::duration<double>(std::min(1.0, config.time_limit * 0.05))));
    current.set_empty_taxi_index(build_empty_taxi_index(current, index_deadline));
    std::vector<std::size_t> initial_order(current.request_count());
    std::iota(initial_order.begin(), initial_order.end(), 0);
    std::shuffle(initial_order.begin(), initial_order.end(), rng);
    for (const auto request : initial_order) {
        if (Clock::now() >= deadline) break;
        const auto choice = best_insertion(current, request);
        if (choice && choice->gain > 1e-9)
            current.insert(request, choice->taxi, choice->pickup, choice->delivery);
    }
    if (config.check) current.check();
    SearchState best = current;

    // T0 and Tmin are derived from config. Both scale together so T0/Tmin ratio is fixed.
    const double T0 = config.temperature;
    const double Tmin = T0 * 1e-4;
    double temperature = T0;

    double anchor_temp = T0;
    double anchor_progress = 0.0;
    double last_improvement_progress = 0.0;
    
    std::size_t iterations = 0;
    if (trace) {
        const double elapsed0 = std::chrono::duration<double>(Clock::now() - start).count();
        trace << "0," << elapsed0 << ",0," << temperature << ','
              << current.benefit() << ',' << best.benefit() << '\n';
    }

    // ── Main SA loop ─────────────────────────────────────────────────────────
    while (true) {
        const double progress = get_progress(iterations);
        if (progress >= 1.0) break;
        if (Clock::now() >= deadline) break;

        const auto selection = balance.select(rng, current.served_count());
        const auto couple = couples[selection.couple];
        SearchState candidate = current;
        const auto removed = destroys[couple.destroy]->apply(
            candidate, selection.removal_count, rng);

        repairs[couple.repair]->apply(candidate, removed, rng);
        if (config.local_search && iterations % config.ls_interval == 0) {
            const double denominator = std::max(1.0, std::abs(best.benefit()));
            const double gap = (best.benefit() - candidate.benefit()) / denominator;
            if (gap <= config.ls_threshold) {
                if (local_search.search_tier1(candidate)) {
                    // re-evaluate gap after tier 1
                    const double new_gap = (best.benefit() - candidate.benefit()) / denominator;
                    if (new_gap <= config.ls_tier2_threshold) {
                        local_search.search_tier2(candidate);
                    }
                } else if (gap <= config.ls_tier2_threshold) {
                    local_search.search_tier2(candidate);
                }
            }
        }

        // Phase 0b: validator hook — only in debug builds with --check.
        if (config.check) candidate.check();

        const double delta = candidate.benefit() - current.benefit();
        const double reward = std::max(0.0, delta / std::max(1.0, std::abs(current.benefit())));
        bool accept = delta >= 0;
        if (!accept) {
            std::uniform_real_distribution<double> uniform(0, 1);
            accept = uniform(rng) < std::exp(delta / temperature);
        }
        if (accept) {
            current = std::move(candidate);
            if (current.benefit() > best.benefit() + 1e-9) {
                best = current;
                std::cerr << "ALNS best iteration=" << iterations + 1
                          << " benefit=" << best.benefit() << '\n';
                last_improvement_progress = progress;
            }
        }
        
        if (progress - last_improvement_progress > 0.15 && progress < 0.85) {
            temperature = T0 * 0.3;
            current = best;
            last_improvement_progress = progress;
            std::cerr << "ALNS reheat at iteration=" << iterations 
                      << " progress=" << static_cast<int>(progress * 100) << "%\n";
        }
        
        balance.update(selection, reward);
        ++iterations;

        temperature = std::max(Tmin, temperature * config.cooling);

        // Phase 0c: profiling log every 1000 iterations.
        if (iterations % 1000 == 0) {
            const double progress = get_progress(iterations);
            const double elapsed = std::chrono::duration<double>(Clock::now() - start).count();
            if (trace) trace << iterations << ',' << elapsed << ',' << progress << ','
                             << temperature << ',' << current.benefit() << ','
                             << best.benefit() << '\n';
            std::cerr << "ALNS iteration=" << iterations
                      << " progress=" << static_cast<int>(progress * 100) << "%"
                      << " temp=" << temperature
                      << " current=" << current.benefit()
                      << " best=" << best.benefit() << '\n';
        }
    }

    if (config.check) best.check();
    if (trace) {
        const double elapsed = std::chrono::duration<double>(Clock::now() - start).count();
        const double progress = get_progress(iterations);
        trace << iterations << ',' << elapsed << ',' << progress << ','
              << temperature << ',' << current.benefit() << ','
              << best.benefit() << '\n';
    }
    for (std::size_t c = 0; c < couples.size() && operator_log; ++c)
        operator_log << destroys[couples[c].destroy]->name() << ','
                     << repairs[couples[c].repair]->name() << ','
                     << balance.calls()[c] << ',' << balance.rewards()[c] << '\n';
    Result result;
    result.solution = best.solution();
    result.benefit = best.benefit();
    result.iterations = iterations;
    result.seconds = std::chrono::duration<double>(Clock::now() - start).count();
    result.annealing_complete = get_progress(iterations) >= 1.0;
    return result;
}
} // namespace soict::alns
