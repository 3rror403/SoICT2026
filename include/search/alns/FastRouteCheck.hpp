#pragma once

#include "search/alns/RouteCache.hpp"

namespace soict::alns {
// Fixed-size feasibility accumulator for a move's unchanged route pieces.
// A piece maps arrival x to departure max(x + duration, ready). Joining pieces
// keeps that map, the latest admissible input, and the parcel-load extrema.
// Travel cost is deliberately absent: each move computes it from changed arcs.
using FastRouteCheck = SegmentAccumulator<false>;
} // namespace soict::alns
