#pragma once

#include "model/Instance.hpp"
#include <iosfwd>

namespace soict {
Instance read_instance(std::istream& in);
void write_instance(std::ostream& out, const Instance& instance);
}
