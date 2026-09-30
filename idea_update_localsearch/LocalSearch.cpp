#include "search/alns/operators/LocalSearch/LocalSearch.hpp"

namespace soict::alns {
namespace {
// Các neighborhood tác động lẫn nhau (ParcelRelocate làm đổi route -> Add/Passenger
// có thể lại cải thiện được). Lặp lại cho tới khi không đổi, nhưng chặn số lượt để
// không tốn thời gian: lượt cuối không cải thiện chỉ tốn cache-lookup vì các operator
// đều có cache theo version của route.
constexpr int kMaxPasses = 3;
} // namespace

bool LocalSearch::search(SearchState& state) const {
    bool any_change = false;
    for (int pass = 0; pass < kMaxPasses; ++pass) {
        bool changed = false;
        for (const auto& move : moves_) {
            changed = move->improve(state) || changed;
        }
        any_change = any_change || changed;
        if (!changed) break;
    }
    return any_change;
}
} // namespace soict::alns
