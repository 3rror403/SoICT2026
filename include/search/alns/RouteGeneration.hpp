#pragma once
#include "search/alns/Config.hpp"
#include "search/alns/operators/Repair/RoutePoolRepair.hpp"

namespace soict::alns {
// Enumeration, sampled-fleet generation and pricing, in their existing order.
void generate_routes(SearchState& current, RoutePoolRepair& pool,
                     const Config& config, Deadline search_deadline);
}
