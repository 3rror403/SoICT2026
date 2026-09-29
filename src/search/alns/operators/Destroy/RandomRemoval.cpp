#include "search/alns/operators/Destroy/RandomRemoval.hpp"

#include <algorithm>
#include <numeric>

namespace soict::alns {
std::vector<std::size_t> RandomRemoval::apply(SearchState& state, std::size_t count,
                                std::mt19937_64& rng) const {
    std::vector<std::size_t> requests(state.request_count());
    std::iota(requests.begin(), requests.end(), 0);
    std::shuffle(requests.begin(), requests.end(), rng);
    std::vector<std::size_t> removed;
    removed.reserve(count);
    for (const auto request : requests) {
        if (removed.size() >= count) break;
        if (state.remove(request)) removed.push_back(request);
    }
    return removed;
}
} // namespace soict::alns
