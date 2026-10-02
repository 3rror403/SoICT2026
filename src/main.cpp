#include "io/SyntheticGenerator.hpp"
#include "io/InstanceIO.hpp"
#include "io/SolutionIO.hpp"
#include "eval/SolutionValidation.hpp"
#include "model/Solution.hpp"
#include "search/alns/ALNS.hpp"
#ifdef SOICT_WITH_CPLEX
#include "search/mip/MIPSolver.hpp"
#endif

#include <cmath>
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
              << "    Optional: --log DIR --temp T --cooling RATE --check --disable-ls --ls-interval N --balance-thompson\n";
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
            const int start = std::string(argv[1]) == "--instance" ? 1 : 2;
            soict::alns::Config config;
            std::string instance_path = stdio ? "-" : "", solution_path = "-";
            for (int i = start; i < argc; ++i) {
                const std::string key(argv[i]);
                if (key == "--check") { config.check = true; continue; }
                if (key == "--disable-ls") { config.local_search = false; continue; }
                if (key == "--balance-thompson") { config.thompson = true; continue; }
                if (i + 1 >= argc) throw std::invalid_argument("option needs a value: " + key);
                const char* value = argv[++i];
                if (key == "--instance" || key == "--input" || key == "-i")
                    instance_path = value;
                else if (key == "--out" || key == "--output" || key == "-o")
                    solution_path = value;
                else if (key == "--seed") config.seed = count_arg(value);
                else if (key == "--ls-interval") {
                    config.ls_interval = count_arg(value);
                    if (!config.ls_interval) throw std::invalid_argument("ls interval must be positive");
                }
                else if (key == "--time-limit" || key == "--time" || key == "-t") {
                    const double requested = alns_positive_arg(value);
                    config.time_limit = requested > 2 ? requested - 1 : requested * 0.9;
                } else if (key == "--temp" || key == "-T")
                    config.temperature = alns_positive_arg(value);
                else if (key == "--cooling") {
                    config.cooling = alns_positive_arg(value);
                    if (config.cooling >= 1) throw std::invalid_argument("cooling must be below 1");
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
            const auto result = soict::alns::solve(instance, config);
            std::ofstream out;
            if (solution_path != "-") {
                out.open(solution_path);
                if (!out) throw std::runtime_error("cannot create solution file");
            }
            soict::write_solution(solution_path == "-" ? std::cout : out, result.solution, instance);
            (solution_path == "-" ? std::cerr : std::cout) << std::setprecision(17) << "benefit=" << result.benefit
                      << " iterations=" << result.iterations
                      << " seconds=" << result.seconds
                      << " stop=" << (result.annealing_complete ? "temperature" : "time_limit")
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
