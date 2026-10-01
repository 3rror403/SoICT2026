#include "search/alns/operators/Destroy/TimeBucketRemoval.hpp"
#include <map>
#include <vector>
#include <random>
#include <algorithm>

namespace soict::alns {
std::vector<std::size_t> TimeBucketRemoval::apply(SearchState& state, std::size_t count,
                                                  std::mt19937_64& rng) const {
    const auto& in = state.instance();
    const double BUCKET_SIZE = 3600.0;
    
    std::map<int, std::vector<std::size_t>> buckets;
    std::vector<int> active_buckets;
    
    for (std::size_t r = 0; r < state.request_count(); ++r) {
        if (state.location(r).taxi < 0) continue;
        
        double t = 0;
        if (r < in.passengers.size()) t = in.passengers[r].pickup.earliest;
        else t = in.parcels[r - in.passengers.size()].pickup.earliest;
        
        int b = static_cast<int>(t / BUCKET_SIZE);
        if (buckets[b].empty()) active_buckets.push_back(b);
        buckets[b].push_back(r);
    }
    
    std::vector<std::size_t> removed;
    if (active_buckets.empty()) return removed;
    
    std::shuffle(active_buckets.begin(), active_buckets.end(), rng);
    
    for (int b : active_buckets) {
        auto& reqs = buckets[b];
        std::shuffle(reqs.begin(), reqs.end(), rng);
        for (auto r : reqs) {
            if (removed.size() >= count) break;
            if (state.remove(r)) removed.push_back(r);
        }
        if (removed.size() >= count) break;
    }
    return removed;
}
} // namespace soict::alns
