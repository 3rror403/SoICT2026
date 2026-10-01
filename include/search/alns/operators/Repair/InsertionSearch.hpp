#pragma once
#include "search/alns/operators/Repair/RepairOperator.hpp"
#include <chrono>
#include <cstdint>
#include <limits>
#include <optional>
#include <utility>
#include <vector>

namespace soict::alns {
using Deadline = std::chrono::steady_clock::time_point;
struct InsertionChoice {
    std::size_t request = 0, taxi = 0, pickup = 0, delivery = 0;
    double gain = -std::numeric_limits<double>::infinity();
    
    bool operator==(const InsertionChoice& o) const {
        return taxi == o.taxi && pickup == o.pickup && delivery == o.delivery;
    }
};
struct InsertionOptions {
    std::vector<InsertionChoice> global_top;
    std::vector<InsertionChoice> route_best;
    
    std::optional<InsertionChoice> best() const {
        if (global_top.empty()) return std::nullopt;
        return global_top[0];
    }
    double second_gain() const {
        if (global_top.size() < 2) return -std::numeric_limits<double>::infinity();
        return global_top[1].gain;
    }
    double third_gain() const {
        if (global_top.size() < 3) return -std::numeric_limits<double>::infinity();
        return global_top[2].gain;
    }
    
    // threshold for considering a new move
    double threshold(std::size_t k) const {
        return global_top.size() >= k ? global_top[k-1].gain : -std::numeric_limits<double>::infinity();
    }
};
struct EmptyTaxiChoice {
    std::size_t taxi = 0;
    double gain = 0;
};
class EmptyTaxiIndex {
public:
    explicit EmptyTaxiIndex(const Instance& instance);
    // Extend an unfinished request's index on first use. A solver process
    // searches in one thread, so this shared cache has no locking overhead.
    bool ensure(std::size_t request, Deadline deadline = Deadline::max()) const;
    [[nodiscard]] bool complete(std::size_t request) const {
        return complete_.at(request);
    }
    [[nodiscard]] const std::vector<EmptyTaxiChoice>& choices(std::size_t request) const {
        return choices_.at(request);
    }
private:
    const Instance* instance_;
    mutable std::vector<std::vector<EmptyTaxiChoice>> choices_;
    mutable std::vector<std::size_t> next_taxi_;
    mutable std::vector<bool> complete_;
};
std::shared_ptr<const EmptyTaxiIndex> build_empty_taxi_index(
    const SearchState& empty_state, Deadline deadline);
InsertionOptions insertion_options(const SearchState& state, std::size_t request);
std::optional<InsertionChoice> best_insertion(const SearchState& state,
                                              std::size_t request);

// Reuse an unserved request's route choice while that route is unchanged.
// A scan made above a gain threshold is reused only when the current best
// meets that threshold; otherwise the route is rescanned exactly.
class BestInsertionCache {
public:
    std::optional<InsertionChoice> best(const SearchState& state,
                                        std::size_t request);
private:
    struct Entry {
        std::uint64_t route_version = 0;
        double threshold = -std::numeric_limits<double>::infinity();
        std::optional<InsertionChoice> best;
    };
    std::vector<std::vector<Entry>> entries_;
};

// Repair only inserts requests, so an accepted insertion changes one taxi.
// Retain the top two positions per request and active taxi; rescan that taxi
// after insertion while keeping the other taxis' positions unchanged.
class RepairInsertionCache {
public:
    [[nodiscard]] static bool enabled_for(const SearchState& state);
    explicit RepairInsertionCache(const SearchState& state);
    [[nodiscard]] InsertionOptions options(std::size_t request) const;
    void refresh_taxi(std::size_t taxi);
private:
    const SearchState& state_;
    using TaxiOptions = std::pair<std::size_t, InsertionOptions>;
    std::vector<std::vector<TaxiOptions>> route_options_;
};
} // namespace soict::alns
