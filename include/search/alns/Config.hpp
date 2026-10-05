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
    double cooling = std::exp(std::log(1e-4) / 60000.0);
    bool check = false;
    bool local_search = true;
    bool clean_cut_2opt_star = true;
    bool all_unserved_repair = true;
    bool profit_repair = true;
    bool route_removal = true;
    bool opportunity_removal = true;
    bool route_pool_repair = true;
    std::size_t ls_interval = 1;
    bool thompson = false;
    std::string log_directory;
};
} // namespace soict::alns
