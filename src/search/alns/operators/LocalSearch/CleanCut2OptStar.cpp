#include "search/alns/operators/LocalSearch/CleanCut2OptStar.hpp"
#include "search/alns/FastRouteCheck.hpp"

#include <stdexcept>
#include <vector>

namespace soict::alns {
namespace {
struct RouteCuts {
    std::size_t taxi;
    std::vector<std::size_t> boundaries;
};

std::vector<std::size_t> clean_boundaries(const SearchState& state, std::size_t taxi) {
    const auto& in = state.instance();
    const auto& route = state.cache(taxi);
    const auto k = in.taxis.size(), n = in.passengers.size(), m = in.parcels.size();
    std::size_t open_parcels = 0;
    std::vector<std::size_t> cuts{0};
    for (std::size_t pos = 0; pos < route.nodes().size(); ++pos) {
        const auto node = route.nodes()[pos];
        if (node >= k + 2 * n && node < k + 2 * n + m) ++open_parcels;
        else if (node >= k + 2 * n + m) --open_parcels;
        if (!open_parcels && route.boundary_allowed(in, pos + 1)) cuts.push_back(pos + 1);
    }
    return cuts;
}
} // namespace

bool CleanCut2OptStar::improve(SearchState& state) const {
    ++calls_;
    const auto& in = state.instance();
    bool changed = false;
    for (;;) {
        std::vector<RouteCuts> routes;
        routes.reserve(state.active_taxis().size());
        for (const auto taxi : state.active_taxis())
            routes.push_back({taxi, clean_boundaries(state, taxi)});
        bool applied = false;
        for (std::size_t i = 0; i < routes.size() && !applied; ++i) {
            for (std::size_t j = i + 1; j < routes.size() && !applied; ++j) {
                const auto taxi_a = routes[i].taxi, taxi_b = routes[j].taxi;
                const auto& a = state.cache(taxi_a);
                const auto& b = state.cache(taxi_b);
                const auto length_a = a.nodes().size(), length_b = b.nodes().size();
                double best_gain = 1e-9;
                std::size_t best_a = 0, best_b = 0;
                for (const auto cut_a : routes[i].boundaries) {
                    for (const auto cut_b : routes[j].boundaries) {
                        const auto prev_a = cut_a ? a.nodes()[cut_a - 1] : taxi_a;
                        const auto prev_b = cut_b ? b.nodes()[cut_b - 1] : taxi_b;
                        double gain = 0;
                        if (cut_a < length_a) {
                            const auto next_a = a.nodes()[cut_a];
                            gain += in.cost(prev_a, next_a) - in.cost(prev_b, next_a);
                        }
                        if (cut_b < length_b) {
                            const auto next_b = b.nodes()[cut_b];
                            gain += in.cost(prev_b, next_b) - in.cost(prev_a, next_b);
                        }
                        if (gain <= best_gain + 1e-8) continue;
                        FastRouteCheck new_a(in, taxi_a, a);
                        new_a.range(0, cut_a);
                        if (cut_b < length_b) new_a.segment(b.range(in, cut_b, length_b - 1));
                        if (!new_a.feasible()) continue;
                        FastRouteCheck new_b(in, taxi_b, b);
                        new_b.range(0, cut_b);
                        if (cut_a < length_a) new_b.segment(a.range(in, cut_a, length_a - 1));
                        if (!new_b.feasible()) continue;
                        best_gain = gain;
                        best_a = cut_a;
                        best_b = cut_b;
                    }
                }
                if (best_gain > 1e-9) {
                    const double before = state.benefit();
                    if (!state.exchange_route_suffixes(taxi_a, best_a, taxi_b, best_b))
                        throw std::logic_error("validated clean-cut exchange failed");
                    ++applied_moves_;
                    total_gain_ += state.benefit() - before;
                    changed = applied = true;
                }
            }
        }
        if (!applied) return changed;
    }
}
} // namespace soict::alns
