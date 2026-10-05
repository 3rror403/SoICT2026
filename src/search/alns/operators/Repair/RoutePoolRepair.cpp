#include "search/alns/operators/Repair/RoutePoolRepair.hpp"
#include <algorithm>
#include <functional>
#include <iomanip>
#include <ostream>
#include <numeric>
#include <stdexcept>

namespace soict::alns {
RoutePoolRepair::RoutePoolRepair(const Instance& instance, std::size_t limit)
    : instance_(instance), limit_(limit), pool_(instance.taxis.size()), versions_(pool_.size(), 0) {
    if (!limit) throw std::invalid_argument("route pool limit must be positive");
}

void RoutePoolRepair::add(const SearchState& state, std::size_t taxi, bool enforce_limit) {
    const auto& nodes = state.cache(taxi).nodes();
    if (nodes.empty()) return;
    const auto k = state.taxi_count(), n = state.passenger_count();
    std::vector<std::size_t> requests;
    double revenue = 0;
    for (const auto node : nodes) {
        std::size_t request;
        if (node < k + n) request = node - k;
        else if (node >= k + 2*n && node < k + 2*n + instance_.parcels.size())
            request = n + node - k - 2*n;
        else continue;
        requests.push_back(request);
        revenue += state.revenue(request);
    }
    std::sort(requests.begin(), requests.end());
    const auto profit = revenue - state.cache(taxi).cost();
    if (profit <= 1e-9) return;
    auto& bank = pool_[taxi];
    const auto found = bank.find(requests);
    if (found != bank.end() && found->second.profit >= profit) return;
    bank[requests] = Column{requests, nodes, profit};
    if (enforce_limit && bank.size() > limit_) {
        const auto worst = std::min_element(bank.begin(), bank.end(), [](const auto& a, const auto& b) {
            return a.second.profit < b.second.profit;
        });
        bank.erase(worst);
    }
}

void RoutePoolRepair::remember(const SearchState& state) {
    if (&state.instance() != &instance_) throw std::logic_error("route pool instance mismatch");
    for (const auto taxi : state.active_taxis()) {
        if (versions_[taxi] == state.cache(taxi).version()) continue;
        versions_[taxi] = state.cache(taxi).version();
        add(state, taxi, true);
    }
}

void RoutePoolRepair::write(std::ostream& out) const {
    out << "taxi,benefit,route\n" << std::setprecision(17);
    for (std::size_t taxi = 0; taxi < pool_.size(); ++taxi)
        for (const auto& [key, column] : pool_[taxi]) {
            out << taxi + 1 << ',' << column.profit << ",\"";
            for (std::size_t i = 0; i < column.route.size(); ++i) {
                if (i) out << ' ';
                out << instance_.vertex_id(column.route[i]);
            }
            out << "\"\n";
        }
}

bool RoutePoolRepair::improve(SearchState& state, Deadline deadline) {
    if (&state.instance() != &instance_) throw std::logic_error("route pool instance mismatch");
    // Restore every incumbent route even if the bank's size cap removed it.
    for (const auto taxi : state.active_taxis()) add(state, taxi, false);
    std::vector<std::vector<Column>> columns(pool_.size());
    std::vector<int> selected(pool_.size(), -1), incumbent(pool_.size(), -1);
    std::vector<unsigned char> occupied(state.request_count(), 0);
    std::vector<std::size_t> frequency(state.request_count(), 0), order;
    column_count_ = 0;
    double best_profit = 0;
    for (std::size_t taxi = 0; taxi < pool_.size(); ++taxi) {
        auto& row = columns[taxi];
        for (const auto& [key, column] : pool_[taxi]) row.push_back(column);
        std::sort(row.begin(), row.end(), [](const auto& a, const auto& b) {
            return a.profit > b.profit || (a.profit == b.profit && a.requests < b.requests);
        });
        column_count_ += row.size();
        std::vector<unsigned char> present(state.request_count(), 0);
        for (std::size_t i = 0; i < row.size(); ++i) {
            for (auto r : row[i].requests) present[r] = 1;
            bool current = !row[i].requests.empty();
            for (auto r : row[i].requests) current &= state.location(r).taxi == static_cast<int>(taxi);
            if (current) {
                // The served-set key must equal the entire incumbent route.
                std::size_t count = 0;
                for (std::size_t r = 0; r < state.request_count(); ++r)
                    count += state.location(r).taxi == static_cast<int>(taxi);
                if (count == row[i].requests.size()) { incumbent[taxi] = static_cast<int>(i); best_profit += row[i].profit; }
            }
        }
        for (std::size_t r = 0; r < present.size(); ++r) frequency[r] += present[r];
    }
    // Constrain the taxis with the most shared requests before independent ones.
    std::vector<std::size_t> conflict(pool_.size(), 0);
    for (std::size_t taxi = 0; taxi < columns.size(); ++taxi)
        for (const auto& column : columns[taxi])
            for (auto r : column.requests) conflict[taxi] += frequency[r] - 1;
    for (std::size_t taxi = 0; taxi < columns.size(); ++taxi)
        if (!columns[taxi].empty()) order.push_back(taxi);
    std::stable_sort(order.begin(), order.end(), [&](auto a, auto b) { return conflict[a] > conflict[b]; });
    const auto compatible = [&](const Column& column) {
        for (auto r : column.requests) if (occupied[r]) return false;
        return true;
    };
    nodes_ = 0; timed_out_ = false;
    // Repair small groups around the feasible incumbent before searching the
    // complete master. This supplies good feasible bounds under a short limit.
    const auto optimize_group = [&](const std::vector<std::size_t>& group) {
        std::fill(occupied.begin(), occupied.end(), 0);
        for (std::size_t taxi = 0; taxi < incumbent.size(); ++taxi) {
            if (incumbent[taxi] < 0 || std::find(group.begin(), group.end(), taxi) != group.end()) continue;
            for (auto r : columns[taxi][incumbent[taxi]].requests) occupied[r] = 1;
        }
        double old_profit = 0;
        std::vector<int> chosen(group.size(), -1), best_group;
        for (auto taxi : group) {
            best_group.push_back(incumbent[taxi]);
            if (incumbent[taxi] >= 0) old_profit += columns[taxi][incumbent[taxi]].profit;
        }
        double group_profit = old_profit;
        std::function<void(std::size_t,double)> visit = [&](std::size_t depth, double profit) {
            if (timed_out_) return;
            ++nodes_;
            if ((nodes_ & 63) == 0 && std::chrono::steady_clock::now() >= deadline) {
                timed_out_ = true; return;
            }
            if (depth == group.size()) {
                if (profit > group_profit + 1e-9) { group_profit = profit; best_group = chosen; }
                return;
            }
            double bound = profit;
            for (std::size_t d = depth; d < group.size(); ++d)
                for (const auto& column : columns[group[d]])
                    if (compatible(column)) { bound += column.profit; break; }
            if (bound <= group_profit + 1e-9) return;
            const auto taxi = group[depth];
            for (std::size_t i = 0; i < columns[taxi].size(); ++i) {
                const auto& column = columns[taxi][i];
                if (!compatible(column)) continue;
                chosen[depth] = static_cast<int>(i);
                for (auto r : column.requests) occupied[r] = 1;
                visit(depth + 1, profit + column.profit);
                for (auto r : column.requests) occupied[r] = 0;
                if (timed_out_) break;
            }
            chosen[depth] = -1;
            if (!timed_out_) visit(depth + 1, profit);
        };
        if (std::chrono::steady_clock::now() < deadline) visit(0,0);
        else timed_out_ = true;
        if (group_profit <= old_profit + 1e-9) return false;
        for (std::size_t i = 0; i < group.size(); ++i) incumbent[group[i]] = best_group[i];
        best_profit += group_profit - old_profit;
        return true;
    };
    bool changed = true;
    while (changed && !timed_out_) {
        changed = false;
        for (std::size_t a = 0; a < order.size() && !timed_out_; ++a)
            for (std::size_t b = a + 1; b < order.size() && !timed_out_; ++b)
                changed = optimize_group({order[a],order[b]}) || changed;
    }
    for (std::size_t a = 0; a < order.size() && !timed_out_; ++a)
        for (std::size_t b = a + 1; b < order.size() && !timed_out_; ++b)
            for (std::size_t c = b + 1; c < order.size() && !timed_out_; ++c)
                optimize_group({order[a],order[b],order[c]});
    // Nonnegative request prices give a valid Lagrangian upper bound:
    // sum(price) + sum_t max(0, profit(route) - sum_request price).
    // Use projected subgradients to tighten it; every iterate is valid, so
    // convergence is unnecessary for correctness or safe early termination.
    std::vector<double> prices(state.request_count(), 0), best_prices = prices;
    double best_dual = std::numeric_limits<double>::infinity();
    std::vector<std::vector<double>> reduced(columns.size());
    std::vector<std::vector<std::size_t>> branches(columns.size());
    for (std::size_t iteration = 0; iteration < 192 && !timed_out_; ++iteration) {
        if (std::chrono::steady_clock::now() >= deadline) { timed_out_ = true; break; }
        double dual = std::accumulate(prices.begin(), prices.end(), 0.0);
        std::vector<std::size_t> uses(prices.size(), 0);
        for (auto taxi : order) {
            double maximum = 0; int chosen = -1;
            for (std::size_t i = 0; i < columns[taxi].size(); ++i) {
                const auto& column = columns[taxi][i];
                double value = column.profit;
                for (auto r : column.requests) value -= prices[r];
                if (value > maximum) { maximum = value; chosen = static_cast<int>(i); }
            }
            dual += maximum;
            if (chosen >= 0) for (auto r : columns[taxi][chosen].requests) ++uses[r];
        }
        if (dual < best_dual) { best_dual = dual; best_prices = prices; }
        double norm = 0;
        for (std::size_t r = 0; r < prices.size(); ++r)
            if (prices[r] > 0 || uses[r] > 1) {
                const double gradient = static_cast<double>(uses[r]) - 1;
                norm += gradient * gradient;
            }
        if (!norm || dual <= best_profit + 1e-7) break;
        const double step = 0.9 * (dual - best_profit) / norm;
        for (std::size_t r = 0; r < prices.size(); ++r)
            prices[r] = std::max(0.0, prices[r] + step * (static_cast<double>(uses[r]) - 1));
    }
    prices = std::move(best_prices);
    const double price_total = std::accumulate(prices.begin(), prices.end(), 0.0);
    for (auto taxi : order) {
        auto& values = reduced[taxi];
        for (const auto& column : columns[taxi]) {
            double value = column.profit;
            for (auto r : column.requests) value -= prices[r];
            values.push_back(value);
        }
        auto& indices = branches[taxi];
        indices.resize(values.size()); std::iota(indices.begin(), indices.end(), 0);
        std::stable_sort(indices.begin(), indices.end(), [&](auto a, auto b) { return values[a] > values[b]; });
    }
    std::fill(occupied.begin(), occupied.end(), 0);
    std::function<void(std::size_t,double,double)> search = [&](std::size_t depth, double profit, double price_used) {
        if (timed_out_) return;
        ++nodes_;
        if ((nodes_ & 63) == 0 && std::chrono::steady_clock::now() >= deadline) {
            timed_out_ = true; return;
        }
        if (depth == order.size()) {
            if (profit > best_profit + 1e-9) { best_profit = profit; incumbent = selected; }
            return;
        }
        double bound = profit;
        for (std::size_t d = depth; d < order.size(); ++d)
            for (const auto& column : columns[order[d]])
                if (compatible(column)) { bound += column.profit; break; }
        if (bound <= best_profit + 1e-9) return;
        double priced_bound = profit + price_total - price_used;
        for (std::size_t d = depth; d < order.size(); ++d) {
            const auto taxi = order[d];
            double maximum = 0;
            for (auto i : branches[taxi]) {
                if (reduced[taxi][i] <= 0) break;
                if (compatible(columns[taxi][i])) { maximum = reduced[taxi][i]; break; }
            }
            priced_bound += maximum;
        }
        // Conservatively tolerate floating-point accumulation in the dual.
        if (priced_bound < best_profit - 1e-7) return;
        const auto taxi = order[depth];
        for (auto i : branches[taxi]) {
            const auto& column = columns[taxi][i];
            if (!compatible(column)) continue;
            selected[taxi] = static_cast<int>(i);
            double used = price_used;
            for (auto r : column.requests) { occupied[r] = 1; used += prices[r]; }
            search(depth + 1, profit + column.profit, used);
            for (auto r : column.requests) occupied[r] = 0;
            if (timed_out_) break;
        }
        selected[taxi] = -1;
        if (!timed_out_) search(depth + 1, profit, price_used); // unused taxi is always permitted
    };
    if (std::chrono::steady_clock::now() < deadline) search(0,0,0);
    else timed_out_ = true;
    if (best_profit <= state.benefit() + 1e-9) return false;
    Solution candidate;
    candidate.routes.resize(state.taxi_count());
    for (std::size_t taxi = 0; taxi < incumbent.size(); ++taxi)
        if (incumbent[taxi] >= 0) candidate.routes[taxi] = columns[taxi][incumbent[taxi]].route;
    const auto evaluated = candidate.evaluate(instance_);
    if (!evaluated.feasible || std::abs(evaluated.benefit - best_profit) > 1e-6)
        throw std::logic_error("route pool selection disagrees with full evaluation");
    SearchState repaired(instance_, std::move(candidate), state.insertion_scratch_handle());
    repaired.set_empty_taxi_index(state.empty_taxi_index());
    state.copy_from(repaired);
    return true;
}
} // namespace soict::alns
