#pragma once

#include <cstdint>
#include <cstddef>
#include <string>

namespace soict::alns {
struct Config {
    std::uint64_t seed = 1;
    double time_limit = 59.0;
    bool check = false;
    bool local_search = true;
    bool clean_cut_2opt_star = true;
    std::size_t path_prefix_limit = 1000000;
    std::size_t route_pool_limit = 400;
    std::size_t path_pool_limit = 400;
    double enumeration_seconds = 2;
    double generation_seconds = 140;
    std::size_t fleet_refinement_interval = 2;
    std::size_t fleet_refinement_cycles = 3;
    bool light_fleet_refinement = true;
    std::string log_directory;
};
} // namespace soict::alns
