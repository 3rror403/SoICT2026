#include "search/alns/RouteGeneration.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>

namespace soict::alns {
void generate_routes(SearchState& current, RoutePoolRepair& pool,
                     const Config& config, Deadline search_deadline) {
    using Clock = std::chrono::steady_clock;
    const auto& instance = current.instance();
    const auto generation_start = Clock::now();
    const auto generation_deadline = std::min(search_deadline, generation_start +
        std::chrono::duration_cast<Clock::duration>(std::chrono::duration<double>(
            std::min(config.enumeration_seconds, config.time_limit * 0.2))));
    pool.generate(generation_deadline, config.path_prefix_limit);
    const auto initial_completed_taxis=pool.completed_taxis();
    const bool initial_enumeration_complete=pool.enumeration_complete();
    const bool sampled = !initial_enumeration_complete || instance.taxis.size()>48;
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
            SearchState generated(instance,pool.generate_fleets(sample_deadline,current.empty_taxi_index(),&initial,
                                  config.local_search,config.clean_cut_2opt_star,config.fleet_refinement_interval,
                                  config.light_fleet_refinement,config.fleet_refinement_cycles),
                                  current.insertion_scratch_handle());
            generated.set_empty_taxi_index(current.empty_taxi_index());current.copy_from(generated);
            if (instance.taxis.size()>48 && Clock::now()+std::chrono::seconds(2)<beam_deadline) {
                const auto mix_deadline=std::min(beam_deadline,Clock::now()+std::chrono::seconds(6));
                pool.improve(current,mix_deadline);
                ++phase;
                std::cerr<<"path_feedback_phase="<<phase<<" path_feedback_benefit="<<current.benefit()<<'\n';
                if (Clock::now()+std::chrono::seconds(2)<beam_deadline) {
                    const auto pricing_start=Clock::now();
                    const auto price_deadline=std::min(beam_deadline,Clock::now()+std::chrono::seconds(3));
                    if (phase%2) pool.generate_replacements(price_deadline,current);
                    else pool.generate(price_deadline,config.path_prefix_limit,true);
                    std::cerr<<"path_pricing_phase="<<phase<<" path_pricing_mode="<<(phase%2?"replacement":"dfs")
                             <<" path_pricing_seconds="<<std::chrono::duration<double>(Clock::now()-pricing_start).count();
                    if (!(phase%2)) std::cerr<<" path_pricing_prefixes="<<pool.generated_prefixes();
                    std::cerr<<'\n';
                }
            }
        } while (instance.taxis.size()>48 && Clock::now()<beam_deadline);
    }
    if (initial_enumeration_complete) {
        if (!sampled) pool.set_limit(std::min(config.route_pool_limit, config.path_pool_limit));
    }
    std::cerr << "path_prefixes=" << pool.total_generated_prefixes()
              << " path_completed_taxis=" << initial_completed_taxis
              << " path_seconds=" << std::chrono::duration<double>(Clock::now()-generation_start).count()
              << '\n';
}
} // namespace soict::alns
