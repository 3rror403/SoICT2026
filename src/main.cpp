#include "io/SyntheticGenerator.hpp"
#include "io/InstanceIO.hpp"
#include "model/Solution.hpp"

#include <fstream>
#include <iostream>
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
void usage() {
    std::cerr << "Usage:\n"
              << "  SoICT2026 generate --passengers N --parcels M --taxis K --seed S --out FILE\n"
              << "  SoICT2026 inspect FILE\n";
}
}

int main(int argc, char** argv) {
    try {
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
