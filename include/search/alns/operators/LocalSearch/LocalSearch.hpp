#pragma once
#include "search/alns/operators/LocalSearch/Move.hpp"
#include <memory>
#include <vector>

namespace soict::alns {
class LocalSearch {
public:
    void add_tier1(std::unique_ptr<Move> move) { tier1_.push_back(std::move(move)); }
    void add_tier2(std::unique_ptr<Move> move) { tier2_.push_back(std::move(move)); }
    void add_tier3(std::unique_ptr<Move> move) { tier3_.push_back(std::move(move)); }
    
    bool search_tier1(SearchState& state) const { return search(state, tier1_); }
    bool search_tier2(SearchState& state) const { return search(state, tier2_); }
    bool search_tier3(SearchState& state) const { return search(state, tier3_); }

private:
    std::vector<std::unique_ptr<Move>> tier1_, tier2_, tier3_;
    bool search(SearchState& state, const std::vector<std::unique_ptr<Move>>& moves) const;
};
} // namespace soict::alns
