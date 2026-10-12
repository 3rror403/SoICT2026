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
    double gain = 0;
};
struct InsertionOptions {
    std::optional<InsertionChoice> best;
    double second_gain = -std::numeric_limits<double>::infinity();
};
struct EmptyTaxiChoice {
    std::size_t taxi = 0;
    double gain = 0;
};
// Immutable request data reused by repeated AddRequest / ProfitInsertion cache
// queries. The instance must remain unchanged while its empty-taxi index lives.
struct RequestSegments {
    bool passenger;
    Segment pickup, delivery;
    double revenue, weight;
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
    [[nodiscard]] const RequestSegments& request_segments(std::size_t request) const {
        return request_segments_.at(request);
    }
    [[nodiscard]] std::size_t request_segment_bytes() const {
        return request_segments_.size() * sizeof(RequestSegments);
    }
private:
    const Instance* instance_;
    mutable std::vector<std::vector<EmptyTaxiChoice>> choices_;
    mutable std::vector<std::size_t> next_taxi_;
    mutable std::vector<bool> complete_;
    // Allocated to the full request count at construction, never resized.
    std::vector<RequestSegments> request_segments_;
};
std::shared_ptr<const EmptyTaxiIndex> build_empty_taxi_index(
    const SearchState& empty_state, Deadline deadline);
InsertionOptions insertion_options(const SearchState& state, std::size_t request);
std::optional<InsertionChoice> best_insertion(const SearchState& state,
                                              std::size_t request);
// The supplied route may be a temporary route with another request removed.
// A non-positive insertion can still participate in an improving replacement.
std::optional<InsertionChoice> best_insertion_on_route(const SearchState& state,
    std::size_t request, std::size_t taxi, const RouteCache& route,
    double minimum_gain = -std::numeric_limits<double>::infinity());

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

// Exact positive choices at a fixed floor, independent of other taxis' gains.
// The dense table and winners are allocated once and reused across repairs.
class PositiveInsertionCache {
public:
    // Refresh only the requested row; unchanged routes retain exact choices.
    const std::optional<InsertionChoice>& best(const SearchState&, std::size_t request);
    void prepare(const SearchState&, std::span<const std::size_t> requests);
    void refresh_taxi(const SearchState&, std::size_t taxi,
                      std::span<const std::size_t> requests);
    [[nodiscard]] const std::optional<InsertionChoice>& best(std::size_t request) const {
        return entries_.at(requests_ * taxis_ + request).choice;
    }
    [[nodiscard]] std::size_t storage_bytes() const { return entries_.size() * sizeof(Entry); }
private:
    struct Entry {
        std::uint64_t version = std::numeric_limits<std::uint64_t>::max();
        std::optional<InsertionChoice> choice;
    };
    std::shared_ptr<const EmptyTaxiIndex> index_;
    std::size_t taxis_ = 0, requests_ = 0;
    // First requests*taxis cells, followed by one winner cell per request.
    std::vector<Entry> entries_;
    void initialize(const SearchState&);
    void refresh_entry(const SearchState&, std::size_t request, std::size_t taxi);
    void select_winner(std::size_t request);
};

// Repair only inserts requests, so an accepted insertion changes one taxi.
// Retain the top two positions per request and active taxi; rescan that taxi
// after insertion while keeping the other taxis' positions unchanged.
class RepairInsertionCache {
public:
    [[nodiscard]] static bool enabled_for(const SearchState& state);
    explicit RepairInsertionCache(const SearchState& state);
    RepairInsertionCache(const SearchState& state, std::span<const std::size_t> requests);
    [[nodiscard]] InsertionOptions options(std::size_t request) const;
    void refresh_taxi(std::size_t taxi);
private:
    const SearchState& state_;
    using TaxiOptions = std::pair<std::size_t, InsertionOptions>;
    std::vector<std::vector<TaxiOptions>> route_options_;
    std::vector<std::size_t> requests_;
};
} // namespace soict::alns
