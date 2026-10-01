#pragma once
#include "search/alns/operators/Repair/RepairOperator.hpp"
#include "search/alns/SearchState.hpp"
#include <vector>
#include <array>

namespace soict::alns {

class SharedGapPairRepair final : public RepairOperator {
public:
    explicit SharedGapPairRepair(const Instance& instance);
    
    std::string_view name() const override { return "pair_aware_greedy"; }
    
    void apply(SearchState& state, std::span<const std::size_t> removed,
               std::mt19937_64& rng) const override;

    // Run the detector immediately after destroy
    void detect_after_destroy(const SearchState& state, std::span<const std::size_t> removed, std::string_view destroy_name) const;

private:
    void precompute_partner_bank(const Instance& instance);

    std::vector<std::vector<std::size_t>> partner_bank_;
    static constexpr std::size_t BANK_SIZE = 8;
};

} // namespace soict::alns
