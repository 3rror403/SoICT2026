#pragma once
#include "search/alns/operators/LocalSearch/Move.hpp"
#include <chrono>
#include <memory>
#include <vector>

namespace soict::alns {
class LocalSearch {
public:
    void add_move(std::unique_ptr<Move> move) { moves_.push_back(std::move(move)); }
    bool search(SearchState& state, std::chrono::steady_clock::time_point deadline =
        std::chrono::steady_clock::time_point::max()) const;
private:
    std::vector<std::unique_ptr<Move>> moves_;
};
} // namespace soict::alns
