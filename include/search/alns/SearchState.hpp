#pragma once

#include "model/Solution.hpp"
#include "search/alns/RouteCache.hpp"
#include <map>
#include <memory>
#include <optional>
#include <cstdint>
#include <vector>

namespace soict::alns {
class EmptyTaxiIndex;

// Request ids 0..N-1 are passengers; N..N+M-1 are parcels.
struct RequestLocation {
    int taxi = -1;
    std::size_t pickup = 0, dropoff = 0;
};

class SearchState {
public:
    SearchState(const Instance& instance, Solution solution);
    [[nodiscard]] Solution solution() const;
    [[nodiscard]] const RouteCache& cache(std::size_t taxi) const;
    [[nodiscard]] const std::map<std::size_t, RouteCache>& active_routes() const {
        return active_routes_;
    }
    [[nodiscard]] std::vector<std::size_t> active_taxis() const;
    [[nodiscard]] bool is_active(std::size_t taxi) const { return active_routes_.contains(taxi); }
    [[nodiscard]] std::size_t taxi_count() const { return instance_->taxis.size(); }
    [[nodiscard]] const RequestLocation& location(std::size_t request) const {
        return locations_.at(request);
    }
    [[nodiscard]] std::size_t request_count() const { return locations_.size(); }
    [[nodiscard]] std::size_t passenger_count() const { return instance_->passengers.size(); }
    [[nodiscard]] std::size_t served_count() const;
    [[nodiscard]] double benefit() const { return benefit_; }
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
    bool insert(std::size_t request, std::size_t taxi,
                std::size_t pickup_boundary, std::size_t delivery_boundary);
    bool relocate_within_route(std::size_t request, std::size_t pickup_boundary,
                               std::size_t delivery_boundary);
    bool apply_clean_cut(std::size_t taxiA, std::size_t cutA,
                         std::size_t taxiB, std::size_t cutB, double delta_benefit);
    void check() const;

private:
    const Instance* instance_;
    // Only used taxis have route state. std::map keeps taxi ids ordered.
    std::map<std::size_t, RouteCache> active_routes_;
    std::vector<RequestLocation> locations_;
    struct RemovalEntry {
        std::uint64_t route_version = 0;
        std::optional<double> gain;
    };
    mutable std::vector<RemovalEntry> removal_cache_;
    std::shared_ptr<const EmptyTaxiIndex> empty_taxi_index_;
    double benefit_ = 0;
    void rebuild_route(std::size_t taxi, std::vector<std::size_t> route);
    [[nodiscard]] std::size_t dropoff_node(std::size_t request) const;
};
} // namespace soict::alns
