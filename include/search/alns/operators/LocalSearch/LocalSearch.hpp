#pragma once
#include "search/alns/operators/LocalSearch/Move.hpp"
#include <memory>
#include <vector>

namespace soict::alns {
class LocalSearch {
public:
    void add_move(std::unique_ptr<Move> move) { moves_.push_back(std::move(move)); }
    bool search(SearchState& state) const;
private:
    std::vector<std::unique_ptr<Move>> moves_;
};
} // namespace soict::alns
