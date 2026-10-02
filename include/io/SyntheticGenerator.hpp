#pragma once

#include "model/Instance.hpp"
#include <cstddef>
#include <cstdint>

namespace soict {

struct GeneratorOptions {
    std::size_t taxis = 10;
    std::size_t passengers = 50;
    std::size_t parcels = 50;
    std::uint64_t seed = 1;
    bool directed_noise = false;
};

Instance generate_synthetic_instance(const GeneratorOptions& options);

} // namespace soict
