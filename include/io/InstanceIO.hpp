#pragma once

#include "model/Instance.hpp"
#include <iosfwd>

namespace soict {
Instance read_instance(std::istream& in);
#ifndef SOICT_SUBMISSION
void write_instance(std::ostream& out, const Instance& instance);
#endif
}
