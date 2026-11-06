#pragma once

#include "model/Solution.hpp"
#include "search/alns/RouteCache.hpp"
#include <memory>
#include <optional>
#include <cstdint>
#include <span>
#include <vector>

namespace soict::alns {
class EmptyTaxiIndex;
class InsertionScratch;

// Request ids 0..N-1 are passengers; N..N+M-1 are parcels.
struct RequestLocation {
    int taxi = -1;
    std::size_t pickup = 0, dropoff = 0;
};

class SearchState {
public:
    SearchState(const Instance& instance, Solution solution,
                std::shared_ptr<InsertionScratch> insertion_scratch = {});
    void copy_from(const SearchState& other);
    void swap(SearchState& other) noexcept;
    [[nodiscard]] Solution solution() const;
    [[nodiscard]] const RouteCache& cache(std::size_t taxi) const;
    [[nodiscard]] const std::vector<std::size_t>& active_taxis() const {
        return active_taxis_;
    }
    [[nodiscard]] bool is_active(std::size_t taxi) const {
        return taxi < routes_by_taxi_.size() && !routes_by_taxi_[taxi].nodes().empty();
    }
    [[nodiscard]] std::size_t taxi_count() const { return instance_->taxis.size(); }
    [[nodiscard]] const RequestLocation& location(std::size_t request) const {
        return locations_.at(request);
    }
    [[nodiscard]] std::size_t request_count() const { return locations_.size(); }
    [[nodiscard]] std::size_t passenger_count() const { return instance_->passengers.size(); }
    [[nodiscard]] std::size_t served_count() const;
    [[nodiscard]] double benefit() const { return benefit_; }
    // State copies share this workspace; insertion scans within a solve are
    // sequential. Scratch contents are not part of the solution or its caches.
    [[nodiscard]] InsertionScratch& insertion_scratch() const {
        return *insertion_scratch_;
    }
    [[nodiscard]] const std::shared_ptr<InsertionScratch>& insertion_scratch_handle() const {
        return insertion_scratch_;
    }
    [[nodiscard]] double revenue(std::size_t request) const;
    [[nodiscard]] const Instance& instance() const { return *instance_; }
    [[nodiscard]] std::size_t pickup_node(std::size_t request) const;
    [[nodiscard]] const std::shared_ptr<const EmptyTaxiIndex>& empty_taxi_index() const {
        return empty_taxi_index_;
    }
    void set_empty_taxi_index(std::shared_ptr<const EmptyTaxiIndex> index) {
        empty_taxi_index_ = std::move(index);
    }
    [[nodiscard]] std::optional<double> removal_gain(std::size_t request) const;
    [[nodiscard]] std::optional<double> insertion_gain(std::size_t request,
        std::size_t taxi, std::size_t pickup_boundary,
        std::size_t delivery_boundary) const;

    bool remove(std::size_t request);
    // Remove complete requests from one taxi atomically. Intermediate shortcut
    // routes need not be feasible on directed/nonmetric travel times.
    bool remove_requests(std::size_t taxi, std::span<const std::size_t> requests);
    bool insert(std::size_t request, std::size_t taxi,
                std::size_t pickup_boundary, std::size_t delivery_boundary);
    bool relocate_within_route(std::size_t request, std::size_t pickup_boundary,
                               std::size_t delivery_boundary);
    // Positions refer to the route after removing outgoing's two endpoints.
    // Only the completed replacement must be feasible and improve benefit.
    bool replace_request(std::size_t outgoing, std::size_t incoming,
                         std::size_t pickup_boundary, std::size_t delivery_boundary);
    // Exchange served requests across taxis, keeping passenger endpoints adjacent.
    bool swap_requests(std::size_t first, std::size_t second);
    // Reject invalid/non-improving exchanges without modifying either route.
    bool exchange_route_suffixes(std::size_t taxi_a, std::size_t cut_a,
                                 std::size_t taxi_b, std::size_t cut_b);
    void check() const;

private:
    const Instance* instance_;
    std::shared_ptr<InsertionScratch> insertion_scratch_;
    // Empty routes retain their buffers for subsequent activation.
    std::vector<RouteCache> routes_by_taxi_;
    // Ordered iteration preserves move tie order and repair-cache sorting.
    std::vector<std::size_t> active_taxis_;
    std::vector<RequestLocation> locations_;
    struct RemovalEntry {
        std::uint64_t route_version = 0;
        std::optional<double> gain;
    };
    mutable std::vector<RemovalEntry> removal_cache_;
    std::shared_ptr<const EmptyTaxiIndex> empty_taxi_index_;
    double benefit_ = 0;
    std::vector<std::size_t> route_scratch_;
    RouteCache relocation_check_;
    void rebuild_route(std::size_t taxi, const std::vector<std::size_t>& route);
    void commit_validated_route(std::size_t taxi, RouteCache& updated, double gain);
    void commit_route_pair(std::size_t taxi_a, RouteCache new_a,
                           std::size_t taxi_b, RouteCache new_b, double gain);
    void set_active(std::size_t taxi, bool active);
    [[nodiscard]] std::size_t dropoff_node(std::size_t request) const;
    void locate_route(std::size_t taxi);
};
} // namespace soict::alns
