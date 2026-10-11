#pragma once

#include <cstdint>
#include <cstddef>
#include <cmath>
#include <string>

namespace soict::alns {
struct Config {
    std::uint64_t seed = 1;
    double time_limit = 59.0;
    double temperature = 200.0;
    // Spend the search budget on collecting diverse routes, then let the
    // final route-pool master combine them. The former 60,000-step schedule
    // can still be selected with --disable-path-generation and
    // --cooling 0.9998465061085267.
    double cooling = std::exp(std::log(1e-4) / 6000.0);
    bool check = false;
    bool local_search = true;
    bool clean_cut_2opt_star = true;
    bool all_unserved_repair = true;
    bool profit_repair = true;
    bool route_removal = true;
    bool opportunity_removal = true;
    bool route_pool_repair = true;
    bool path_generation = true;
    bool paths_only = false;
    std::size_t path_prefix_limit = 50000;
    std::size_t route_pool_limit = 200;
    std::size_t path_pool_limit = 48;
    std::size_t ls_interval = 1;
    bool thompson = false;
    std::string log_directory;
};
} // namespace soict::alns
