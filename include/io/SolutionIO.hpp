#pragma once

#include "model/Solution.hpp"
#include <iosfwd>

namespace soict {

#ifndef SOICT_SUBMISSION
Solution read_solution(std::istream& in, const Instance& instance);
#endif
void write_solution(std::ostream& out, const Solution& solution, const Instance& instance);

} // namespace soict
