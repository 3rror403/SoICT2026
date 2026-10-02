#include "search/alns/operators/LocalSearch/CleanCut2OptStar.hpp"
#include "search/alns/FastRouteCheck.hpp"

#include <vector>
#include <limits>

namespace soict::alns {
namespace {
struct CleanCut {
    std::size_t pos;
};

struct RouteCuts {
    std::size_t taxi;
    std::vector<CleanCut> cuts;
};
} // namespace

bool CleanCut2OptStar::improve(SearchState& state) const {
    const auto& in = state.instance();
    bool changed = false;

    // Outer loop to keep applying moves as long as we find improvements
    while (true) {
        std::vector<RouteCuts> active_cuts;
        for (std::size_t taxi : state.active_taxis()) {
            const auto& route = state.cache(taxi);
            const auto length = route.nodes().size();
            RouteCuts rc;
            rc.taxi = taxi;
            
            for (std::size_t pos = 0; pos <= length; ++pos) {
                if (!route.boundary_allowed(in, pos)) continue;
                if (pos > 0 && route.load()[pos - 1] > 1e-8) continue;
                rc.cuts.push_back({pos});
            }
            active_cuts.push_back(std::move(rc));
        }
        
        bool move_applied = false;
        
        for (std::size_t i = 0; i < active_cuts.size() && !move_applied; ++i) {
            for (std::size_t j = i + 1; j < active_cuts.size() && !move_applied; ++j) {
                const auto taxiA = active_cuts[i].taxi;
                const auto taxiB = active_cuts[j].taxi;
                const auto& routeA = state.cache(taxiA);
                const auto& routeB = state.cache(taxiB);
                const auto lengthA = routeA.nodes().size();
                const auto lengthB = routeB.nodes().size();
                
                double best_delta = 0.0;
                std::size_t best_cutA = 0, best_cutB = 0;
                bool found = false;
                
                for (const auto& cutA : active_cuts[i].cuts) {
                    for (const auto& cutB : active_cuts[j].cuts) {
                        const auto prevA = cutA.pos == 0 ? taxiA : routeA.nodes()[cutA.pos - 1];
                        const auto nextA = cutA.pos == lengthA ? std::numeric_limits<std::size_t>::max() : routeA.nodes()[cutA.pos];
                        
                        const auto prevB = cutB.pos == 0 ? taxiB : routeB.nodes()[cutB.pos - 1];
                        const auto nextB = cutB.pos == lengthB ? std::numeric_limits<std::size_t>::max() : routeB.nodes()[cutB.pos];
                        
                        double old_cost = 0;
                        if (nextA != std::numeric_limits<std::size_t>::max()) old_cost += in.cost(prevA, nextA);
                        if (nextB != std::numeric_limits<std::size_t>::max()) old_cost += in.cost(prevB, nextB);
                        
                        double new_cost = 0;
                        if (nextB != std::numeric_limits<std::size_t>::max()) new_cost += in.cost(prevA, nextB);
                        if (nextA != std::numeric_limits<std::size_t>::max()) new_cost += in.cost(prevB, nextA);
                        
                        const double delta_cost = old_cost - new_cost;
                        if (delta_cost <= best_delta + 1e-8) continue;
                        
                        FastRouteCheck newA(in, taxiA, routeA);
                        if (cutA.pos > 0) newA.range(0, cutA.pos);
                        if (cutB.pos < lengthB) newA.segment(routeB.range(in, cutB.pos, lengthB - 1));
                        if (!newA.feasible()) continue;
                        
                        FastRouteCheck newB(in, taxiB, routeB);
                        if (cutB.pos > 0) newB.range(0, cutB.pos);
                        if (cutA.pos < lengthA) newB.segment(routeA.range(in, cutA.pos, lengthA - 1));
                        if (!newB.feasible()) continue;
                        
                        best_delta = delta_cost;
                        best_cutA = cutA.pos;
                        best_cutB = cutB.pos;
                        found = true;
                    }
                }
                
                if (found) {
                    if (state.apply_clean_cut(taxiA, best_cutA, taxiB, best_cutB, best_delta)) {
                        changed = true;
                        move_applied = true;
                    }
                }
            }
        }
        
        if (!move_applied) break;
    }
    
    return changed;
}
} // namespace soict::alns
