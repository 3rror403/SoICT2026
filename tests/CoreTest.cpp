#include "io/SyntheticGenerator.hpp"
#include "io/InstanceIO.hpp"
#include "io/SolutionIO.hpp"
#include "model/Solution.hpp"

#include <cmath>
#include <sstream>
#include <stdexcept>

namespace {
void require(bool condition) {
    if (!condition) throw std::runtime_error("core test failed");
}
}

int main() {
    using namespace soict;
    GeneratorOptions options;
    options.taxis = 2;
    options.passengers = 2;
    options.parcels = 2;
    options.seed = 42;
    const auto generated = generate_synthetic_instance(options);
    for (const auto& passenger : generated.passengers)
        require(std::abs((passenger.pickup.latest - passenger.pickup.earliest) - 25.0) < 1e-9);
    for (const auto& parcel : generated.parcels) {
        require(std::abs((parcel.pickup.latest - parcel.pickup.earliest) - 70.0) < 1e-9);
        require(std::abs((parcel.delivery.latest - parcel.delivery.earliest) - 30.0) < 1e-9);
    }
    std::stringstream first, second;
    write_instance(first, generated);
    write_instance(second, generate_synthetic_instance(options));
    require(first.str() == second.str());
    auto parsed = read_instance(first);
    require(parsed.node_count() == 10);
    std::stringstream roundtrip;
    write_instance(roundtrip, parsed);
    require(roundtrip.str() == second.str());

    Instance instance;
    instance.taxis = {{2}, {2}};
    instance.passengers = {{20, {10, 20}, 1}};
    instance.parcels = {{12, 2, {0, 20}, {20, 40}, 1}};
    const auto v = instance.node_count();
    instance.travel_time.assign(v * v, 0);
    instance.travel_cost.assign(v * v, 0);
    auto arc = [&](std::size_t a, std::size_t b, double t, double c) {
        instance.travel_time[a * v + b] = t;
        instance.travel_cost[a * v + b] = c;
    };
    const auto pp = instance.passenger_pickup(0);
    const auto pd = instance.passenger_dropoff(0);
    const auto fp = instance.parcel_pickup(0);
    const auto fd = instance.parcel_dropoff(0);
    arc(0, fp, 1, 1);
    arc(fp, pp, 1, 1);
    arc(pp, pd, 2, 2);
    arc(pd, fd, 1, 1);
    instance.validate();
    Solution valid{{{fp, pp, pd, fd}, {}}};
    const auto result = valid.evaluate(instance);
    require(result.feasible);
    require(std::abs(result.benefit - 27.0) < 1e-9);
    std::stringstream saved_solution;
    write_solution(saved_solution, valid);
    require(saved_solution.str() == "SOICT2026_SOLUTION 3\n2 1\n0 4 2 3 5\n");
    require(std::abs(read_solution(saved_solution).evaluate(instance).benefit - 27.0) < 1e-9);
    const Solution sparse{{{}, {pp, pd}, {}, {fp, fd}}};
    std::stringstream sparse_file;
    write_solution(sparse_file, sparse);
    require(sparse_file.str() == "SOICT2026_SOLUTION 3\n4 2\n1 2 3\n3 4 5\n");
    require(read_solution(sparse_file).routes == sparse.routes);
    std::stringstream empty_file;
    write_solution(empty_file, Solution{{{}, {}}});
    require(empty_file.str() == "SOICT2026_SOLUTION 3\n2 0\n");
    require(read_solution(empty_file).routes == Solution{{{}, {}}}.routes);
    std::stringstream legacy("SOICT2026_SOLUTION 1\n2\n4 4 2 3 5\n0\n");
    require(read_solution(legacy).routes == valid.routes);
    std::stringstream version_two("SOICT2026_SOLUTION 2\n2 1\n0 4 4 2 3 5\n");
    require(read_solution(version_two).routes == valid.routes);
    for (const char* bad : {
             "SOICT2026_SOLUTION 3\n2 2\n0 4\n0 5\n",
             "SOICT2026_SOLUTION 3\n2 2\n1 4\n0 5\n",
             "SOICT2026_SOLUTION 3\n2 1\n2 4\n",
             "SOICT2026_SOLUTION 3\n2 1\n0\n",
             "SOICT2026_SOLUTION 3\n2 1\n0 4 x\n",
             "SOICT2026_SOLUTION 3\n2 0\n0 4\n",
             "SOICT2026_SOLUTION 3\n2 1 extra\n0 4\n",
             "SOICT2026_SOLUTION 2\n2 1\n0 2 4\n"}) {
        std::stringstream malformed(bad);
        bool rejected = false;
        try { (void)read_solution(malformed); }
        catch (const std::runtime_error&) { rejected = true; }
        require(rejected);
    }
    // Waiting at an early passenger pickup delays the subsequent delivery.
    instance.parcels[0].delivery.latest = 14;
    require(!valid.evaluate(instance).feasible);
    instance.parcels[0].delivery.latest = 40;
    // Waiting at an early parcel delivery also delays later stops.
    require(!Solution{{{fp, fd, pp, pd}, {}}}.evaluate(instance).feasible);
    // Passenger drop-offs have no latest arrival time.
    arc(pp, pd, 100, 2);
    require(Solution{{{pp, pd}, {}}}.evaluate(instance).feasible);
    arc(pp, pd, 2, 2);
    require(!Solution{{{pp, fp, pd, fd}, {}}}.evaluate(instance).feasible);
    require(!Solution{{{fp, pp, pd}, {fd}}}.evaluate(instance).feasible);
    instance.taxis[0].parcel_capacity = 1;
    require(!valid.evaluate(instance).feasible);
    instance.taxis[0].parcel_capacity = 2;
    instance.passengers[0].pickup.latest = 2;
    require(!valid.evaluate(instance).feasible);
}
