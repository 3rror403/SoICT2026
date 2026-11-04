#pragma once
#include "search/alns/operators/Repair/InsertionSearch.hpp"
#include <map>
#include <iosfwd>

namespace soict::alns {
// A restricted fleet repair over complete, independently feasible taxi routes.
// Identical served sets on one taxi retain only the most profitable sequence.
class RoutePoolRepair {
public:
    explicit RoutePoolRepair(const Instance& instance, std::size_t limit = 200, std::uint64_t seed = 1);
    void remember(const SearchState& state);
    // Enumerate feasible prefixes independently for each taxi. Only completed
    // positive-profit routes enter the existing capped, served-set bank.
    // Completion refers to prefix enumeration, not retention of every column.
    void generate(Deadline deadline, std::size_t prefix_limit = 50000, bool priced = false);
    // Perturb complete routes and repair with cached insertion. Preserve all
    // sampled alternatives and return the best feasible sampled fleet.
    Solution generate_fleets(Deadline deadline, std::shared_ptr<const EmptyTaxiIndex> index = {},
                             const Solution* initial = nullptr, bool refine = true,
                             bool clean_cut = true, std::size_t refinement_interval = 2,
                             bool light_refinement = true, std::size_t refinement_cycles = 3);
    void generate_replacements(Deadline deadline, const SearchState& fleet);
    void set_limit(std::size_t limit);
    std::size_t generated_prefixes() const { return generated_prefixes_; }
    std::size_t total_generated_prefixes() const { return total_generated_prefixes_; }
    std::size_t completed_taxis() const { return completed_taxis_; }
    bool enumeration_complete() const { return completed_taxis_ == pool_.size(); }
    bool improve(SearchState& state, Deadline deadline);
    // Statistics and completion describe the most recent improve() call,
    // before any subsequent generation changes that call's route bank.
    std::size_t columns() const { return column_count_; }
    std::size_t nodes() const { return nodes_; }
    bool timed_out() const { return timed_out_; }
    bool master_complete() const { return complete_; }
    void write(std::ostream& out) const;
private:
    struct Column {
        // The bank key owns the served set; store only its route and profit here.
        std::vector<std::size_t> route;
        double profit = 0;
    };
    using Bank = std::map<std::vector<std::size_t>, Column>;
    struct Arc { std::size_t request; double latest_departure; };
    const Instance& instance_;
    std::size_t limit_, column_count_ = 0, nodes_ = 0;
    bool timed_out_ = false;
    bool complete_ = false;
    std::size_t generated_prefixes_ = 0, completed_taxis_ = 0;
    std::size_t total_generated_prefixes_ = 0;
    std::vector<double> floors_;
    std::uint64_t seed_ = 1;
    std::uint64_t fleet_calls_ = 0;
    std::uint64_t pricing_calls_ = 0;
    std::uint64_t generation_calls_ = 0;
    std::vector<Bank> pool_;
    std::vector<std::uint64_t> versions_;
    std::vector<std::vector<Arc>> next_;
    std::vector<double> delivery_lower_;
    bool generation_index_ready_ = false;
    std::uint64_t bank_revision_ = 0;
    mutable std::uint64_t prices_revision_ = std::numeric_limits<std::uint64_t>::max();
    mutable std::vector<double> cached_prices_;
    std::vector<std::size_t> request_scratch_;
    void add(const SearchState&, std::size_t taxi, bool enforce_limit);
    void add_column(std::size_t taxi, const std::vector<std::size_t>& requests,
                    const std::vector<std::size_t>& route, double profit, bool enforce_limit);
    std::vector<double> estimate_prices(Deadline deadline = Deadline::min()) const;
};
} // namespace soict::alns
