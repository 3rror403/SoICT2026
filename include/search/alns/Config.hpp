#pragma once

#include <cstdint>
#include <cstddef>
#include <cmath>
#include <optional>
#include <string>

namespace soict::alns {
struct Config {
    std::uint64_t seed = 1;
    double time_limit = 59.0;
    double temperature = 10.0;
    double cooling = std::exp(std::log(1e-4) / 60000.0);
    bool check = false;
    bool local_search = true;
    std::size_t ls_interval = 10;
    double ls_threshold = 0.10;
    double ls_tier2_threshold = 0.02;
    bool thompson = false;
    std::string log_directory;
    // Phase 0d: deterministic mode — if set, loop runs exactly N iterations
    // using cooling = (Tmin/T0)^(1/N) via shared progress variable.
    std::optional<std::size_t> max_iterations;
};
} // namespace soict::alns
