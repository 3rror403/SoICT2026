#pragma once
#include "search/alns/operators/Repair/InsertionSearch.hpp"
#include <map>
#include <iosfwd>

namespace soict::alns {
// A restricted fleet repair over complete, independently feasible taxi routes.
// Identical served sets on one taxi retain only the most profitable sequence.
class RoutePoolRepair {
public:
    explicit RoutePoolRepair(const Instance& instance, std::size_t limit = 200);
    void remember(const SearchState& state);
    // Enumerate feasible prefixes independently for each taxi. Only completed
    // positive-profit routes enter the existing capped, served-set bank.
    // Completion refers to prefix enumeration, not retention of every column.
    void generate(Deadline deadline, std::size_t prefix_limit = 50000);
    void set_limit(std::size_t limit);
    std::size_t generated_prefixes() const { return generated_prefixes_; }
    std::size_t completed_taxis() const { return completed_taxis_; }
    bool enumeration_complete() const { return completed_taxis_ == pool_.size(); }
    bool improve(SearchState& state, Deadline deadline);
    std::size_t columns() const { return column_count_; }
    std::size_t nodes() const { return nodes_; }
    bool timed_out() const { return timed_out_; }
    void write(std::ostream& out) const;
private:
    struct Column {
        std::vector<std::size_t> requests, route;
        double profit = 0;
    };
    const Instance& instance_;
    std::size_t limit_, column_count_ = 0, nodes_ = 0;
    bool timed_out_ = false;
    std::size_t generated_prefixes_ = 0, completed_taxis_ = 0;
    std::vector<std::map<std::vector<std::size_t>, Column>> pool_;
    std::vector<std::uint64_t> versions_;
    void add(const SearchState&, std::size_t taxi, bool enforce_limit);
    void add_column(std::size_t taxi, const std::vector<std::size_t>& requests,
                    const std::vector<std::size_t>& route, double profit, bool enforce_limit);
};
} // namespace soict::alns
