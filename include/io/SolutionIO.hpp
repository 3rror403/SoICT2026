#pragma once

#include "model/Solution.hpp"
#include <iosfwd>

namespace soict {

Solution read_solution(std::istream& in);
void write_solution(std::ostream& out, const Solution& solution);

} // namespace soict
