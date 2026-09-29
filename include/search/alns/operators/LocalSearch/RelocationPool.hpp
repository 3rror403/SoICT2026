#pragma once

#include "search/alns/operators/Repair/InsertionSearch.hpp"
#include <algorithm>
#include <optional>
#include <vector>

namespace soict::alns {
// For a fixed request and destination route, only its best position can win
// best improvement. A route change invalidates every descriptor that reads it.
struct RelocationDescriptor {
    InsertionChoice choice;
    std::size_t source;
};

class RelocationPool {
public:
    void add(std::optional<InsertionChoice> choice, std::size_t source) {
        if (choice && choice->gain > 1e-9) moves_.push_back({*choice, source});
    }
    [[nodiscard]] std::optional<RelocationDescriptor> best() const {
        if (moves_.empty()) return std::nullopt;
        return *std::max_element(moves_.begin(), moves_.end(),
            [](const auto& a, const auto& b) {
                return a.choice.gain < b.choice.gain;
            });
    }
    void erase_affected(std::size_t first, std::size_t second) {
        std::erase_if(moves_, [&](const auto& move) {
            return move.source == first || move.source == second ||
                   move.choice.taxi == first || move.choice.taxi == second;
        });
    }
    void clear() { moves_.clear(); }
private:
    std::vector<RelocationDescriptor> moves_;
};
} // namespace soict::alns
