#include "search/alns/SearchState.hpp"
#include "search/alns/operators/LocalSearch/CleanCut2OptStar.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>
#include <stdexcept>

namespace {
void require(bool condition) {
    if (!condition) throw std::runtime_error("clean-cut differential test failed");
}
}

int main() {
    using namespace soict;
    std::mt19937_64 rng(20261003);
    std::size_t checked = 0, accepted = 0;
    for (std::size_t trial = 0; trial < 120; ++trial) {
        Instance in;
        in.taxis = {{3}, {2}, {1}};
        in.passengers.resize(3);
        in.parcels.resize(4);
        for (auto& p : in.passengers) p = {200, {double(rng() % 20), 2000}, 1};
        for (std::size_t p = 0; p < 4; ++p)
            in.parcels[p] = {200, p % 2 ? 1.0 : 2.0,
                            {double(rng() % 20), 2000}, {0, 2000}, 1};
        const auto v = in.node_count();
        in.travel_time.resize(v * v);
        in.travel_cost.resize(v * v);
        for (std::size_t u = 0; u < v; ++u)
            for (std::size_t w = 0; w < v; ++w) {
                in.travel_time[u * v + w] = u == w ? 0 : 1 + rng() % 25;
                in.travel_cost[u * v + w] = u == w ? 0 : 1 + rng() % 50;
            }
        Solution initial;
        initial.routes = {
            {in.passenger_pickup(0), in.passenger_dropoff(0),
             in.parcel_pickup(0), in.parcel_pickup(1), in.parcel_dropoff(0), in.parcel_dropoff(1)},
            {in.parcel_pickup(2), in.passenger_pickup(1), in.passenger_dropoff(1), in.parcel_dropoff(2)},
            {in.passenger_pickup(2), in.passenger_dropoff(2), in.parcel_pickup(3), in.parcel_dropoff(3)}};
        if (trial % 2) {
            for (std::size_t taxi = 0; taxi < 3; ++taxi) {
                const alns::RouteCache route(in, taxi, initial.routes[taxi]);
                for (std::size_t pos = 0; pos < route.nodes().size(); ++pos) {
                    const auto node = route.nodes()[pos];
                    const double latest = route.arrival()[pos] + rng() % 8;
                    if (node < 6) in.passengers[node - 3].pickup.latest = std::max(latest, in.passengers[node - 3].pickup.earliest);
                    else if (node >= 9 && node < 13) in.parcels[node - 9].pickup.latest = std::max(latest, in.parcels[node - 9].pickup.earliest);
                    else if (node >= 13) in.parcels[node - 13].delivery.latest = latest;
                }
            }
        }
        in.validate();
        alns::SearchState original(in, initial);
        original.check();
        for (std::size_t a = 0; a < 3; ++a) {
            for (std::size_t b = a + 1; b < 3; ++b) {
                for (std::size_t cut_a = 0; cut_a <= initial.routes[a].size(); ++cut_a) {
                    for (std::size_t cut_b = 0; cut_b <= initial.routes[b].size(); ++cut_b) {
                        auto expected = initial;
                        expected.routes[a].assign(initial.routes[a].begin(), initial.routes[a].begin() + cut_a);
                        expected.routes[a].insert(expected.routes[a].end(), initial.routes[b].begin() + cut_b, initial.routes[b].end());
                        expected.routes[b].assign(initial.routes[b].begin(), initial.routes[b].begin() + cut_b);
                        expected.routes[b].insert(expected.routes[b].end(), initial.routes[a].begin() + cut_a, initial.routes[a].end());
                        const auto full = expected.evaluate(in);
                        auto actual = original;
                        const bool moved = actual.exchange_route_suffixes(a, cut_a, b, cut_b);
                        require(moved == (full.feasible && full.benefit > original.benefit() + 1e-9));
                        require(actual.solution().routes == (moved ? expected.routes : initial.routes));
                        require(std::abs(actual.benefit() - (moved ? full.benefit : original.benefit())) < 1e-7);
                        require(actual.served_count() == original.served_count());
                        require(std::is_sorted(actual.active_taxis().begin(), actual.active_taxis().end()));
                        actual.check();
                        // Also check location indices, version changes, and subsequent move deltas.
                        for (std::size_t request = 0; request < actual.request_count(); ++request) {
                            const auto loc = actual.location(request);
                            const auto& nodes = actual.cache(loc.taxi).nodes();
                            require(nodes[loc.pickup] == actual.pickup_node(request));
                            auto without = actual.solution();
                            auto& route = without.routes[loc.taxi];
                            route.erase(route.begin() + loc.dropoff);
                            route.erase(route.begin() + loc.pickup);
                            const auto removal = without.evaluate(in);
                            const auto gain = actual.removal_gain(request);
                            require(bool(gain) == removal.feasible);
                            if (gain) require(std::abs(*gain - (removal.benefit - actual.benefit())) < 1e-7);
                        }
                        for (std::size_t taxi = 0; taxi < 3; ++taxi) {
                            if (!moved || (taxi != a && taxi != b))
                                require(actual.cache(taxi).version() == original.cache(taxi).version());
                        }
                        ++checked;
                        accepted += moved;
                    }
                }
            }
        }
        auto searched = original;
        alns::CleanCut2OptStar move;
        move.improve(searched);
        require(searched.benefit() >= original.benefit());
        require(searched.served_count() == original.served_count());
        searched.check();
        require(!move.improve(searched));
        require(!searched.exchange_route_suffixes(0, 0, 0, 0));
        require(!searched.exchange_route_suffixes(3, 0, 0, 0));
        require(!searched.exchange_route_suffixes(0, 999, 1, 0));
        searched.check();
    }
    require(accepted > 0);
    std::cout << "checked=" << checked << " accepted=" << accepted << '\n';
}
