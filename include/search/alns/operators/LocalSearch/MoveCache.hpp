#pragma once
#include "search/alns/operators/Repair/InsertionSearch.hpp"
#include <cstdint>
#include <optional>
#include <vector>

namespace soict::alns {
// A positive best position on another taxi is also its cheapest feasible
// insertion, so it remains best when only the source route changes. A scan
// finding no positive move proves that fact for any smaller source saving.
// Same-route moves depend on the source route's exact version.
class MoveCache {
public:
    template<class Scan>
    std::optional<InsertionChoice> get(std::size_t request, std::size_t taxi,
        std::size_t taxi_count, std::uint64_t source_version,
        std::uint64_t target_version, double source_saving,
        bool same_route, Scan&& scan) {
        if (entries_.size() <= request) entries_.resize(request + 1);
        auto& row = entries_[request];
        if (row.size() < taxi_count) row.resize(taxi_count);
        auto& entry = row[taxi];
        const bool matching_target = entry.valid &&
            entry.target_version == target_version &&
            entry.same_route == same_route;
        const bool reuse = matching_target && (same_route ?
            entry.source_version == source_version :
            (entry.choice.has_value() || source_saving <= entry.source_saving));
        if (!reuse) {
            entry.choice = scan();
            entry.source_version = source_version;
            entry.target_version = target_version;
            entry.source_saving = source_saving;
            entry.same_route = same_route;
            entry.valid = true;
        }
        if (entry.choice && !same_route) {
            auto adjusted = *entry.choice;
            adjusted.gain += source_saving - entry.source_saving;
            return adjusted;
        }
        return entry.choice;
    }
private:
    struct Entry {
        std::optional<InsertionChoice> choice;
        std::uint64_t source_version = 0, target_version = 0;
        double source_saving = 0;
        bool same_route = false;
        bool valid = false;
    };
    std::vector<std::vector<Entry>> entries_;
};
} // namespace soict::alns
