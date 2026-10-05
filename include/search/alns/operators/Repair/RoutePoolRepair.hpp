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
    std::vector<std::map<std::vector<std::size_t>, Column>> pool_;
    std::vector<std::uint64_t> versions_;
    void add(const SearchState&, std::size_t taxi, bool enforce_limit);
};
} // namespace soict::alns
