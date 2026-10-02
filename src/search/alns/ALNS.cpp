#include "search/alns/ALNS.hpp"
#include "search/alns/Balance.hpp"
#include "search/alns/SearchState.hpp"
#include "search/alns/operators/Destroy/RandomRemoval.hpp"
#include "search/alns/operators/Destroy/RelatedRemoval.hpp"
#include "search/alns/operators/Destroy/WorstRemoval.hpp"
#include "search/alns/operators/LocalSearch/LocalSearch.hpp"
#include "search/alns/operators/LocalSearch/ParcelRelocate.hpp"
#include "search/alns/operators/LocalSearch/PassengerRelocate.hpp"
#include "search/alns/operators/LocalSearch/AddRequest.hpp"
#include "search/alns/operators/LocalSearch/RemoveRequest.hpp"
#include "search/alns/operators/Repair/GreedyInsertion.hpp"
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
    std::mt19937_64 rng(config.seed);
    std::vector<std::unique_ptr<DestroyOperator>> destroys;
    destroys.push_back(std::make_unique<RandomRemoval>());
    destroys.push_back(std::make_unique<WorstRemoval>());
    destroys.push_back(std::make_unique<RelatedRemoval>());
    std::vector<std::unique_ptr<RepairOperator>> repairs;
    repairs.push_back(std::make_unique<GreedyInsertion>());
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
    SearchState candidate = current;
    double temperature = config.temperature;
    const double minimum_temperature = config.temperature * 1e-4;
    std::size_t iterations = 0;
    if (trace) trace << "0,0," << temperature << ',' << current.benefit() << ','
                     << best.benefit() << '\n';

    while (temperature > minimum_temperature) {
        const auto iteration_start = Clock::now();
        if (iteration_start >= deadline) break;
        const auto selection = balance.select(rng, current.served_count());
        const auto couple = couples[selection.couple];
        candidate.copy_from(current);
        const auto removed = destroys[couple.destroy]->apply(
            candidate, selection.removal_count, rng);
        repairs[couple.repair]->apply(candidate, removed, rng);
        if (config.local_search && iterations % config.ls_interval == 0)
            local_search.search(candidate);
        if (config.check) candidate.check();
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
                std::cerr << "ALNS best iteration=" << iterations + 1
                          << " benefit=" << best.benefit() << '\n';
            }
        }
        balance.update(selection, reward);
        ++iterations;
        temperature = std::max(minimum_temperature, temperature * config.cooling);
        if (iterations % 1000 == 0) {
            const double seconds = std::chrono::duration<double>(iteration_start - start).count();
            if (trace) trace << iterations << ',' << seconds << ',' << temperature << ','
                             << current.benefit() << ',' << best.benefit() << '\n';
            std::cerr << "ALNS iteration=" << iterations << " temp=" << temperature
                      << " current=" << current.benefit() << " best=" << best.benefit() << '\n';
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
    return result;
}
} // namespace soict::alns
