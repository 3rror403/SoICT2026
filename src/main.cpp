#include "io/SyntheticGenerator.hpp"
#include "io/InstanceIO.hpp"
#include "io/SolutionIO.hpp"
#include "eval/SolutionValidation.hpp"
#include "model/Solution.hpp"
#include "search/alns/ALNS.hpp"
#include "search/alns/operators/Repair/RoutePoolMaster.hpp"
#ifdef SOICT_WITH_CPLEX
#include "search/mip/MIPSolver.hpp"
#endif

#include <cmath>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <string>

namespace {
std::size_t count_arg(const char* value) {
    const std::string text(value);
    std::size_t position = 0;
    const auto count = std::stoull(text, &position);
    if (position != text.size()) throw std::invalid_argument("invalid integer: " + text);
    return count;
}
#ifdef SOICT_WITH_CPLEX
double positive_arg(const char* value) {
    const std::string text(value);
    std::size_t position = 0;
    const double number = std::stod(text, &position);
    if (position != text.size() || !std::isfinite(number) || number <= 0)
        throw std::invalid_argument("invalid positive number: " + text);
    return number;
}
#endif
double alns_positive_arg(const char* value) {
    const std::string text(value);
    std::size_t position = 0;
    const double number = std::stod(text, &position);
    if (position != text.size() || !std::isfinite(number) || number <= 0)
        throw std::invalid_argument("invalid positive number: " + text);
    return number;
}
void usage() {
    std::cerr << "Usage:\n"
              << "  SoICT2026 generate --passengers N --parcels M --taxis K --seed S [--profile empirical|directed-noise] --out FILE\n"
              << "  SoICT2026 inspect FILE\n";
    std::cerr << "  SoICT2026 check-solution INSTANCE SOLUTION\n";
    std::cerr << "  SoICT2026 solve-alns --instance FILE --seed S --time-limit SEC [--out SOLUTION]\n"
              << "  SoICT2026 solve-stdio [--seed S] [--time-limit SEC] < INSTANCE > SOLUTION\n"
              << "  SoICT2026 --instance FILE --seed S --time-limit SEC [--out SOLUTION]\n"
              << "    Omit --out or use --out - to print the solution to stdout.\n"
              << "    Optional: --log DIR --check --disable-ls\n";
    std::cerr << "    Clean-cut suffix exchange is enabled by default; --disable-clean-cut-2opt-star disables it.\n";
    std::cerr << "    Default: bounded route generation and conflict-free fleet selection.\n"
              << "    Large or incomplete banks use sampled fleets, priced routes, and repeated fleet selection.\n"
              << "    Route generation: --path-prefix-limit N --path-pool-limit N\n"
              << "    Generation defaults: 1,000,000 prefixes and 400 routes per taxi; enumeration shares at most two seconds.\n"
              << "    --enumeration-seconds controls DFS; --generation-seconds controls route sampling.\n"
              << "    --fleet-refinement-interval N refines every N sampled fleets (default two).\n"
              << "    Candidate sampling uses light refinement; --full-fleet-refinement adds relocation and swap.\n"
              << "    --fleet-refinement-cycles N sets sampling refinement cycles (default three).\n"
              << "    Route bank: --route-pool-limit N\n";
    std::cerr << "    Large-bank backend: " << (soict::alns::route_master_uses_cplex()?"CPLEX (one thread)":"native C++") << ".\n"
              << "    CLI time limits include input parsing and reserve two seconds for final output and cleanup.\n";
#ifdef SOICT_WITH_CPLEX
    std::cerr << "  SoICT2026 solve-mip FILE [--time-limit SECONDS] [--out SOLUTION]\n";
#endif
}
}

int main(int argc, char** argv) {
    try {
        if (argc == 2 && std::string(argv[1]) == "--help") {
            usage();
            return 0;
        }
        if (argc >= 2 && (std::string(argv[1]) == "solve-alns" ||
                          std::string(argv[1]) == "solve-stdio" ||
                          std::string(argv[1]) == "--instance")) {
            const bool stdio = std::string(argv[1]) == "solve-stdio";
            const auto solve_start = std::chrono::steady_clock::now();
            const int start = std::string(argv[1]) == "--instance" ? 1 : 2;
            soict::alns::Config config;
            std::string instance_path = stdio ? "-" : "", solution_path = "-";
            for (int i = start; i < argc; ++i) {
                const std::string key(argv[i]);
                if (key == "--check") { config.check = true; continue; }
                if (key == "--disable-ls") { config.local_search = false; continue; }
                if (key == "--clean-cut-2opt-star") { config.clean_cut_2opt_star = true; continue; }
                if (key == "--disable-clean-cut-2opt-star") { config.clean_cut_2opt_star = false; continue; }
                if (key == "--full-fleet-refinement") { config.light_fleet_refinement = false; continue; }
                if (key == "--light-fleet-refinement") { config.light_fleet_refinement = true; continue; }
                if (i + 1 >= argc) throw std::invalid_argument("option needs a value: " + key);
                const char* value = argv[++i];
                if (key == "--instance" || key == "--input" || key == "-i")
                    instance_path = value;
                else if (key == "--out" || key == "--output" || key == "-o")
                    solution_path = value;
                else if (key == "--seed") config.seed = count_arg(value);
                else if (key == "--path-prefix-limit") {
                    config.path_prefix_limit = count_arg(value);
                    if (!config.path_prefix_limit) throw std::invalid_argument("path prefix limit must be positive");
                }
                else if (key == "--fleet-refinement-cycles") {
                    config.fleet_refinement_cycles=count_arg(value);
                    if (!config.fleet_refinement_cycles) throw std::invalid_argument("fleet refinement cycles must be positive");
                }
                else if (key == "--fleet-refinement-interval") {
                    config.fleet_refinement_interval=count_arg(value);
                    if (!config.fleet_refinement_interval) throw std::invalid_argument("fleet refinement interval must be positive");
                }
                else if (key == "--generation-seconds") config.generation_seconds = alns_positive_arg(value);
                else if (key == "--enumeration-seconds") config.enumeration_seconds = alns_positive_arg(value);
                else if (key == "--route-pool-limit") {
                    config.route_pool_limit = count_arg(value);
                    if (!config.route_pool_limit) throw std::invalid_argument("route pool limit must be positive");
                }
                else if (key == "--path-pool-limit") {
                    config.path_pool_limit = count_arg(value);
                    if (!config.path_pool_limit) throw std::invalid_argument("path pool limit must be positive");
                }
                else if (key == "--time-limit" || key == "--time" || key == "-t") {
                    const double requested = alns_positive_arg(value);
                    config.time_limit = requested > 3 ? requested - 2 : requested * 0.9;
                } else if (key == "--log") config.log_directory = value;
                else throw std::invalid_argument("unknown solve-alns option: " + key);
            }
            if (instance_path.empty())
                throw std::invalid_argument("solve-alns requires --instance");
            std::ifstream in;
            if (instance_path != "-") {
                in.open(instance_path);
                if (!in) throw std::runtime_error("cannot open instance file");
            }
            const auto instance = soict::read_instance(instance_path == "-" ? std::cin : in);
            config.time_limit = std::max(0.0,config.time_limit-
                std::chrono::duration<double>(std::chrono::steady_clock::now()-solve_start).count());
            const auto result = soict::alns::solve(instance, config);
            std::ofstream out;
            if (solution_path != "-") {
                out.open(solution_path);
                if (!out) throw std::runtime_error("cannot create solution file");
            }
            soict::write_solution(solution_path == "-" ? std::cout : out, result.solution, instance);
            (solution_path == "-" ? std::cerr : std::cout) << std::setprecision(17) << "benefit=" << result.benefit
                      << " iterations=0"
                      << " seconds=" << result.seconds
                      << " stop=" << (result.timed_out ? "time_limit" : "route_pool")
                      << '\n';
            return 0;
        }
#ifdef SOICT_WITH_CPLEX
        if (argc >= 3 && std::string(argv[1]) == "solve-mip") {
            double time_limit = 300;
            std::string solution_path;
            for (int i = 3; i < argc; i += 2) {
                if (i + 1 >= argc) throw std::invalid_argument("option needs a value");
                const std::string key(argv[i]);
                if (key == "--time-limit") time_limit = positive_arg(argv[i + 1]);
                else if (key == "--out") solution_path = argv[i + 1];
                else throw std::invalid_argument("unknown solve-mip option: " + key);
            }
            std::ifstream in(argv[2]);
            if (!in) throw std::runtime_error("cannot open instance file");
            const auto instance = soict::read_instance(in);
            const auto result = soict::solve_mip(instance, time_limit);
            if (result.has_solution && !solution_path.empty()) {
                std::ofstream out(solution_path);
                if (!out) throw std::runtime_error("cannot create solution file");
                soict::write_solution(out, result.solution, instance);
            }
            std::cout << std::setprecision(17);
            std::cout << "status=" << (result.optimal ? "optimal" :
                                           result.has_solution ? "feasible" : "no_solution")
                      << " seconds=" << result.seconds;
            if (result.has_solution) {
                std::cout << " benefit=" << result.benefit
                          << " best_bound=" << result.best_bound << '\n';
                for (std::size_t k = 0; k < result.solution.routes.size(); ++k) {
                    const auto& route = result.solution.routes[k];
                    if (route.empty()) continue;
                    std::cout << "taxi=" << k << " route=";
                    for (const auto node : route) std::cout << ' ' << node;
                    std::cout << '\n';
                }
            } else std::cout << '\n';
            return result.optimal ? 0 : 3;
        }
#endif
        if (argc == 4 && std::string(argv[1]) == "check-solution") {
            std::ifstream input(argv[2]), output(argv[3]);
            if (!input || !output) throw std::runtime_error("cannot open instance or solution file");
            const auto instance = soict::read_instance(input);
            const auto solution = soict::read_solution(output, instance);
            std::cout << std::setprecision(17)
                      << "benefit=" << soict::validate_solution(instance, solution) << '\n';
            return 0;
        }
        if (argc >= 3 && std::string(argv[1]) == "inspect" && argc == 3) {
            std::ifstream in(argv[2]);
            if (!in) throw std::runtime_error("cannot open instance file");
            const auto instance = soict::read_instance(in);
            soict::Solution empty;
            empty.routes.resize(instance.taxis.size());
            const auto result = empty.evaluate(instance);
            if (!result.feasible) throw std::runtime_error(result.error);
            std::cout << "taxis=" << instance.taxis.size()
                      << " passengers=" << instance.passengers.size()
                      << " parcels=" << instance.parcels.size()
                      << " nodes=" << instance.node_count()
                      << " empty_benefit=" << result.benefit << '\n';
            return 0;
        }
        if (argc >= 2 && std::string(argv[1]) == "generate") {
            soict::GeneratorOptions options;
            std::string output;
            for (int i = 2; i < argc; i += 2) {
                if (i + 1 >= argc) throw std::invalid_argument("option needs a value");
                const std::string key(argv[i]);
                if (key == "--passengers") options.passengers = count_arg(argv[i + 1]);
                else if (key == "--parcels") options.parcels = count_arg(argv[i + 1]);
                else if (key == "--taxis") options.taxis = count_arg(argv[i + 1]);
                else if (key == "--seed") options.seed = count_arg(argv[i + 1]);
                else if (key == "--out") output = argv[i + 1];
                else if (key == "--profile") {
                    const std::string profile(argv[i + 1]);
                    if (profile != "empirical" && profile != "directed-noise")
                        throw std::invalid_argument("unknown generation profile: " + profile);
                    options.directed_noise = profile == "directed-noise";
                }
                else throw std::invalid_argument("unknown option: " + key);
            }
            if (output.empty()) throw std::invalid_argument("--out is required");
            const auto instance = soict::generate_synthetic_instance(options);
            std::ofstream out(output);
            if (!out) throw std::runtime_error("cannot create output file");
            soict::write_instance(out, instance);
            std::cout << "wrote " << output << " with " << instance.node_count()
                      << " nodes\n";
            return 0;
        }
        usage();
        return 2;
    } catch (const std::exception& error) {
        std::cerr << "error: " << error.what() << '\n';
        return 1;
    }
}
