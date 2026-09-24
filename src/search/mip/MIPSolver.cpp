#include "search/mip/MIPSolver.hpp"

#include <ilcplex/ilocplex.h>

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

ILOSTLBEGIN

namespace soict {
MIPResult solve_mip(const Instance& instance, double time_limit_seconds) {
    instance.validate();
    const int k_count = static_cast<int>(instance.taxis.size());
    const int n = static_cast<int>(instance.passengers.size());
    const int m = static_cast<int>(instance.parcels.size());
    const int requests = n + m;
    const int s = 2 * requests;
    if (requests > 10) throw std::invalid_argument("MIP is limited to 10 requests");
    if (!std::isfinite(time_limit_seconds) || time_limit_seconds <= 0)
        throw std::invalid_argument("MIP time limit must be positive and finite");

    const auto request_of = [&](int v) {
        if (v < n) return v;
        if (v < 2 * n) return v - n;
        if (v < 2 * n + m) return n + v - 2 * n;
        return n + v - 2 * n - m;
    };
    const auto is_pickup = [&](int v) {
        return v < n || (v >= 2 * n && v < 2 * n + m);
    };
    const auto duration = [&](int v) {
        return request_of(v) < n ? instance.passengers[request_of(v)].service
                                 : instance.parcels[request_of(v) - n].service;
    };
    const auto load_change = [&](int v) {
        if (v < 2 * n) return 0.0;
        if (v < 2 * n + m) return instance.parcels[v - 2 * n].weight;
        return -instance.parcels[v - 2 * n - m].weight;
    };
    const auto earliest = [&](int v) {
        if (v < n) return instance.passengers[v].pickup.earliest;
        if (v < 2 * n) return 0.0;
        if (v < 2 * n + m) return instance.parcels[v - 2 * n].pickup.earliest;
        return instance.parcels[v - 2 * n - m].delivery.earliest;
    };
    const auto latest = [&](int v) {
        if (v < n) return instance.passengers[v].pickup.latest;
        if (v < 2 * n) return -1.0; // No passenger drop-off window.
        if (v < 2 * n + m) return instance.parcels[v - 2 * n].pickup.latest;
        return instance.parcels[v - 2 * n - m].delivery.latest;
    };
    const auto node = [&](int v) { return static_cast<std::size_t>(k_count + v); };

    double max_travel = 0, max_service = 0, max_latest = 0, max_capacity = 0, max_weight = 0;
    for (double t : instance.travel_time) max_travel = std::max(max_travel, t);
    for (const auto& taxi : instance.taxis)
        max_capacity = std::max(max_capacity, taxi.parcel_capacity);
    for (const auto& p : instance.passengers) max_service = std::max(max_service, p.service);
    for (const auto& p : instance.parcels) {
        max_service = std::max(max_service, p.service);
        max_weight = std::max(max_weight, p.weight);
    }
    for (int v = 0; v < s; ++v) max_latest = std::max(max_latest, latest(v));
    const double horizon = max_latest + (s + 1) * (max_travel + max_service) + 1;
    const double time_m = horizon + max_service + max_travel;
    const double load_m = max_capacity + max_weight;

    IloEnv env;
    try {
        IloModel model(env);
        IloBoolVarArray served(env, requests);
        IloBoolVarArray start(env, k_count * s);
        IloBoolVarArray arc(env, s * s);
        IloBoolVarArray finish(env, s);
        IloNumVarArray arrival(env), service(env), order(env), vehicle(env), capacity(env), load(env);
        for (int v = 0; v < s; ++v) {
            arrival.add(IloNumVar(env, 0, horizon));
            service.add(IloNumVar(env, 0, horizon));
            order.add(IloNumVar(env, 0, s));
            vehicle.add(IloNumVar(env, 0, k_count - 1));
            capacity.add(IloNumVar(env, 0, max_capacity));
            load.add(IloNumVar(env, 0, max_capacity));
        }
        const auto y = [&](int k, int v) { return start[k * s + v]; };
        const auto x = [&](int u, int v) { return arc[u * s + v]; };

        IloExpr objective(env);
        for (int r = 0; r < requests; ++r) {
            const double revenue = r < n ? instance.passengers[r].revenue
                                         : instance.parcels[r - n].revenue;
            objective += revenue * served[r];
        }
        for (int k = 0; k < k_count; ++k) {
            IloExpr departures(env);
            for (int v = 0; v < s; ++v) {
                if (!is_pickup(v)) y(k, v).setUB(0);
                departures += y(k, v);
                objective -= instance.cost(k, node(v)) * y(k, v);
                model.add(arrival[v] >= instance.time(k, node(v)) - time_m * (1 - y(k, v)));
                model.add(vehicle[v] - k <= k_count * (1 - y(k, v)));
                model.add(k - vehicle[v] <= k_count * (1 - y(k, v)));
                model.add(capacity[v] - instance.taxis[k].parcel_capacity <=
                          max_capacity * (1 - y(k, v)));
                model.add(instance.taxis[k].parcel_capacity - capacity[v] <=
                          max_capacity * (1 - y(k, v)));
                model.add(load[v] - load_change(v) <= load_m * (1 - y(k, v)));
                model.add(load_change(v) - load[v] <= load_m * (1 - y(k, v)));
            }
            model.add(departures <= 1);
            departures.end();
        }
        for (int u = 0; u < s; ++u) {
            for (int v = 0; v < s; ++v) {
                const bool wrong_passenger_successor = u < n && v != n + u;
                const bool wrong_passenger_predecessor = v >= n && v < 2 * n && u != v - n;
                if (u == v || wrong_passenger_successor || wrong_passenger_predecessor)
                    x(u, v).setUB(0);
                objective -= instance.cost(node(u), node(v)) * x(u, v);
                model.add(arrival[v] >= service[u] + duration(u) +
                          instance.time(node(u), node(v)) - time_m * (1 - x(u, v)));
                model.add(order[v] >= order[u] + 1 - (s + 1) * (1 - x(u, v)));
                model.add(vehicle[v] - vehicle[u] <= k_count * (1 - x(u, v)));
                model.add(vehicle[u] - vehicle[v] <= k_count * (1 - x(u, v)));
                model.add(capacity[v] - capacity[u] <= max_capacity * (1 - x(u, v)));
                model.add(capacity[u] - capacity[v] <= max_capacity * (1 - x(u, v)));
                model.add(load[v] - load[u] - load_change(v) <= load_m * (1 - x(u, v)));
                model.add(load[u] + load_change(v) - load[v] <= load_m * (1 - x(u, v)));
            }
        }
        for (int v = 0; v < s; ++v) {
            const auto visit = served[request_of(v)];
            IloExpr incoming(env), outgoing(env);
            for (int k = 0; k < k_count; ++k) incoming += y(k, v);
            for (int u = 0; u < s; ++u) incoming += x(u, v);
            for (int w = 0; w < s; ++w) outgoing += x(v, w);
            outgoing += finish[v];
            model.add(incoming == visit);
            model.add(outgoing == visit);
            incoming.end();
            outgoing.end();

            model.add(arrival[v] <= horizon * visit);
            model.add(service[v] <= horizon * visit);
            model.add(service[v] >= arrival[v]);
            if (v >= n && v < 2 * n) model.add(service[v] == arrival[v]);
            else {
                model.add(service[v] >= earliest(v) * visit);
                model.add(arrival[v] <= latest(v) * visit);
            }
            model.add(order[v] <= s * visit);
            model.add(order[v] >= visit);
            model.add(vehicle[v] <= (k_count - 1) * visit);
            model.add(capacity[v] <= max_capacity * visit);
            model.add(load[v] <= max_capacity * visit);
            model.add(load[v] <= capacity[v]);
        }
        for (int i = 0; i < n; ++i)
            model.add(x(i, n + i) == served[i]);
        for (int j = 0; j < m; ++j) {
            const int p = 2 * n + j, d = 2 * n + m + j;
            model.add(order[d] >= order[p] + 1 - (s + 1) * (1 - served[n + j]));
            model.add(vehicle[d] - vehicle[p] <= k_count * (1 - served[n + j]));
            model.add(vehicle[p] - vehicle[d] <= k_count * (1 - served[n + j]));
        }
        model.add(IloMaximize(env, objective));
        objective.end();

        IloCplex cplex(model);
        cplex.setOut(env.getNullStream());
        cplex.setParam(IloCplex::Param::TimeLimit, time_limit_seconds);
        cplex.setParam(IloCplex::Param::Threads, 1);
        cplex.setParam(IloCplex::Param::MIP::Tolerances::MIPGap, 0.0);
        const bool found = cplex.solve();
        MIPResult result;
        result.has_solution = found;
        result.optimal = cplex.getStatus() == IloAlgorithm::Optimal;
        result.seconds = cplex.getTime();
        if (found) {
            result.benefit = cplex.getObjValue();
            result.best_bound = cplex.getBestObjValue();
            result.solution.routes.resize(k_count);
            std::vector<int> next(s, -1);
            for (int u = 0; u < s; ++u)
                for (int v = 0; v < s; ++v)
                    if (cplex.getValue(x(u, v)) > 0.5) next[u] = v;
            for (int k = 0; k < k_count; ++k) {
                int first = -1;
                for (int v = 0; v < s; ++v)
                    if (cplex.getValue(y(k, v)) > 0.5) first = v;
                for (int v = first, count = 0; v != -1; v = next[v]) {
                    if (++count > s) throw std::runtime_error("MIP route contains a cycle");
                    result.solution.routes[k].push_back(node(v));
                }
            }
            const auto check = result.solution.evaluate(instance);
            if (!check.feasible) throw std::runtime_error("MIP route failed evaluation: " + check.error);
            if (std::abs(check.benefit - result.benefit) >
                1e-5 * (1 + std::abs(result.benefit)))
                throw std::runtime_error("MIP objective differs from route evaluation");
        }
        env.end();
        return result;
    } catch (const IloException& error) {
        const std::string message = error.getMessage();
        env.end();
        throw std::runtime_error("CPLEX: " + message);
    } catch (...) {
        env.end();
        throw;
    }
}
} // namespace soict
