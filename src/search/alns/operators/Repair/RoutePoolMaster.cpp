#include "search/alns/operators/Repair/RoutePoolMaster.hpp"
#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>

namespace soict::alns {
#ifdef SOICT_ROUTE_POOL_CPLEX
std::vector<double> cplex_route_prices(const std::vector<MasterColumn>&,
    std::size_t, std::size_t, Deadline);
MasterResult cplex_route_master(const std::vector<MasterColumn>&,
    std::size_t, std::size_t, const std::vector<int>&, Deadline);
#endif
namespace {
struct PackingMatrix {
    const std::vector<MasterColumn>& columns;
    std::vector<std::vector<std::size_t>> rows;
    std::vector<std::size_t> resources;
    std::size_t dimension = 0, taxis;
    PackingMatrix(const std::vector<MasterColumn>& c, std::size_t k, std::size_t requests)
        : columns(c), rows(c.size()), taxis(k) {
        std::vector<int> index(k+requests,-1);
        for (std::size_t j = 0; j < c.size(); ++j) {
            const auto add = [&](std::size_t r) {
                if (index[r] < 0) {index[r] = static_cast<int>(dimension++);resources.push_back(r);}
                rows[j].push_back(index[r]);
            };
            add(c[j].taxi); for (auto r : c[j].requests) add(k+r);
        }
    }
};

// A child adds x_j <= 0 or -x_j <= -1 to its parent's packing LP.
// The parent's dual remains feasible, so dual simplex restores primal
// feasibility while reusing the inverse basis and prices.
class PackingLP {
    const PackingMatrix* matrix_;
    std::size_t n_;
    std::vector<double> inverse_, basic_, prices_, rhs_, signs_;
    std::vector<std::size_t> basis_;
    std::vector<unsigned char> in_basis_;
    std::vector<int> bound_row_;

    double coefficient(std::size_t row, std::size_t column) const {
        const auto nc = matrix_->columns.size();
        if (column >= nc) return inverse_[row*n_+column-nc];
        double value = 0;
        for (auto r : matrix_->rows[column]) value += inverse_[row*n_+r];
        if (bound_row_[column] >= 0) {
            const auto r = static_cast<std::size_t>(bound_row_[column]);
            value += signs_[r]*inverse_[row*n_+r];
        }
        return value;
    }
    double reduced(std::size_t column) const {
        const auto nc = matrix_->columns.size();
        if (column >= nc) return -prices_[column-nc];
        double value = matrix_->columns[column].profit;
        for (auto r : matrix_->rows[column]) value -= prices_[r];
        if (bound_row_[column] >= 0) {
            const auto r = static_cast<std::size_t>(bound_row_[column]);
            value -= signs_[r]*prices_[r];
        }
        return value;
    }
    void pivot(std::size_t leaving, std::size_t entering) {
        std::vector<double> direction(n_), row(n_);
        for (std::size_t i = 0; i < n_; ++i) direction[i] = coefficient(i,entering);
        const auto divisor = direction[leaving];
        const double step = basic_[leaving]/divisor, gain = reduced(entering);
        for (std::size_t r = 0; r < n_; ++r) {
            row[r] = inverse_[leaving*n_+r]/divisor;
            prices_[r] += gain*row[r];
        }
        for (std::size_t i = 0; i < n_; ++i) if (i != leaving) {
            basic_[i] -= step*direction[i];
            if (direction[i] == 0) continue;
            for (std::size_t r = 0; r < n_; ++r) inverse_[i*n_+r] -= direction[i]*row[r];
        }
        std::copy(row.begin(),row.end(),inverse_.begin()+leaving*n_);
        basic_[leaving] = step;
        in_basis_[basis_[leaving]] = 0;
        basis_[leaving] = entering; in_basis_[entering] = 1;
        ++pivots;
    }
public:
    std::size_t pivots = 0;
    bool timed_out = false;
    explicit PackingLP(const PackingMatrix& matrix) : matrix_(&matrix), n_(matrix.dimension),
        inverse_(n_*n_,0), basic_(n_,1), prices_(n_,0), rhs_(n_,1), signs_(n_,0),
        basis_(n_), in_basis_(matrix.columns.size()+n_,0), bound_row_(matrix.columns.size(),-1) {
        for (std::size_t i = 0; i < n_; ++i) {
            inverse_[i*n_+i] = 1;
            basis_[i] = matrix.columns.size()+i; in_basis_[basis_[i]] = 1;
        }
    }
    void primal(Deadline deadline) {
        const auto nc = matrix_->columns.size();
        std::size_t degenerate = 0;
        for (std::size_t iteration = 0; iteration < 10000; ++iteration) {
            if ((iteration & 7) == 0 && std::chrono::steady_clock::now() >= deadline) {
                timed_out = true; return;
            }
            std::size_t entering = nc+n_;
            double gain = 1e-8;
            for (std::size_t j = 0; j < nc+n_; ++j) {
                if (in_basis_[j]) continue;
                const double value = reduced(j);
                if (value > gain) {
                    gain = value; entering = j;
                    if (degenerate > n_) break;
                }
            }
            if (entering == nc+n_) return;
            std::size_t leaving = n_;
            double step = std::numeric_limits<double>::infinity();
            for (std::size_t i = 0; i < n_; ++i) {
                const auto d = coefficient(i,entering);
                if (d <= 1e-9) continue;
                const auto ratio = std::max(0.0,basic_[i])/d;
                if (ratio < step-1e-9 || (std::abs(ratio-step) <= 1e-9 &&
                    (leaving == n_ || basis_[i] < basis_[leaving]))) {
                    leaving = i; step = ratio;
                }
            }
            if (leaving == n_ || !std::isfinite(step)) return;
            degenerate = step < 1e-9 ? degenerate+1 : 0;
            pivot(leaving,entering);
        }
    }
    void constrain(std::size_t column, bool include) {
        const double sign = include ? -1 : 1, rhs = include ? -1 : 0;
        const auto old = n_++;
        std::vector<double> enlarged(n_*n_,0);
        for (std::size_t i = 0; i < old; ++i)
            std::copy_n(inverse_.begin()+i*old,old,enlarged.begin()+i*n_);
        double value = 0;
        for (std::size_t i = 0; i < old; ++i) if (basis_[i] == column) {
            value = basic_[i];
            for (std::size_t r = 0; r < old; ++r) enlarged[old*n_+r] = -sign*inverse_[i*old+r];
            break;
        }
        enlarged[old*n_+old] = 1; inverse_ = std::move(enlarged);
        basic_.push_back(rhs-sign*value); prices_.push_back(0); rhs_.push_back(rhs); signs_.push_back(sign);
        basis_.push_back(matrix_->columns.size()+old); in_basis_.push_back(1);
        bound_row_[column] = static_cast<int>(old); pivots = 0;
    }
    void dual(Deadline deadline) {
        const auto nc = matrix_->columns.size();
        for (std::size_t iteration = 0; iteration < 10000; ++iteration) {
            if ((iteration & 7) == 0 && std::chrono::steady_clock::now() >= deadline) {
                timed_out = true; return;
            }
            std::size_t leaving = n_;
            double minimum = -1e-7;
            for (std::size_t i = 0; i < n_; ++i)
                if (basic_[i] < minimum) { minimum = basic_[i]; leaving = i; }
            if (leaving == n_) return;
            std::size_t entering = nc+n_;
            double ratio = std::numeric_limits<double>::infinity();
            for (std::size_t j = 0; j < nc+n_; ++j) {
                if (in_basis_[j]) continue;
                const auto d = coefficient(leaving,j);
                if (d >= -1e-9) continue;
                const double candidate = reduced(j)/d;
                if (candidate < ratio-1e-9) { entering = j; ratio = candidate; }
            }
            // A numerical stop is never used as an infeasibility proof. The
            // checked price bound remains valid and branching is exhaustive.
            if (entering == nc+n_) return;
            pivot(leaving,entering);
        }
    }
    std::vector<double> values() const {
        std::vector<double> x(matrix_->columns.size(),0);
        for (std::size_t i = 0; i < n_; ++i)
            if (basis_[i] < x.size() && std::isfinite(basic_[i]))
                x[basis_[i]] = std::clamp(basic_[i],0.0,1.0);
        return x;
    }
    double upper() const {
        // Any nonnegative prices give a valid bound, including branch-row
        // prices. Check residual profit for every column independently.
        auto prices = prices_;
        double bound = 0, magnitude = 0;
        for (std::size_t r = 0; r < n_; ++r) {
            if (!std::isfinite(prices[r])) return std::numeric_limits<double>::infinity();
            prices[r] = std::max(0.0,prices[r]);
            bound += prices[r]*rhs_[r]; magnitude += std::abs(prices[r]*rhs_[r]);
        }
        std::vector<double> residual(matrix_->taxis,0);
        for (std::size_t j = 0; j < matrix_->columns.size(); ++j) {
            const auto& c = matrix_->columns[j];
            double value = c.profit;
            for (auto r : matrix_->rows[j]) value -= prices[r];
            if (bound_row_[j] >= 0) {
                const auto r = static_cast<std::size_t>(bound_row_[j]);
                value -= signs_[r]*prices[r];
            }
            residual[c.taxi] = std::max(residual[c.taxi],value);
        }
        const auto remainder = std::accumulate(residual.begin(),residual.end(),0.0);
        bound += remainder; magnitude += remainder;
        if (!std::isfinite(bound) || !std::isfinite(magnitude))
            return std::numeric_limits<double>::infinity();
        return bound+1e-7+1e-10*magnitude;
    }
    std::vector<double> request_prices(std::size_t requests) const {
        std::vector<double> result(requests,0);
        for (std::size_t row=0;row<matrix_->resources.size();++row) {
            const auto resource=matrix_->resources[row];
            if (resource>=matrix_->taxis && std::isfinite(prices_[row]))
                result[resource-matrix_->taxis]=std::max(0.0,prices_[row]);
        }
        return result;
    }
};
}

std::vector<double> route_master_prices(const std::vector<MasterColumn>& columns,
        std::size_t taxis, std::size_t requests, Deadline deadline) {
#ifdef SOICT_ROUTE_POOL_CPLEX
    try {return cplex_route_prices(columns,taxis,requests,deadline);}
    catch (const std::exception& error) {std::cerr<<"route_pool_cplex_error="<<error.what()<<'\n';}
#endif
    const PackingMatrix matrix(columns,taxis,requests);
    PackingLP root(matrix);root.primal(deadline);
    std::cerr<<"path_price_lp_pivots="<<root.pivots<<'\n';
    return root.request_prices(requests);
}

bool route_master_uses_cplex() {
#ifdef SOICT_ROUTE_POOL_CPLEX
    return true;
#else
    return false;
#endif
}

MasterResult solve_route_master(const std::vector<MasterColumn>& columns,
        std::size_t taxis, std::size_t requests, const std::vector<int>& incumbent,
        Deadline deadline, bool verbose) {
    MasterResult result; result.selected = incumbent;
    for (auto id : incumbent) if (id >= 0) result.profit += columns[id].profit;
    if (columns.empty()) {result.complete=true;return result;}
    const PackingMatrix matrix(columns,taxis,requests);
    PackingLP root(matrix); root.primal(deadline);
    result.pivots = root.pivots;
    if (root.timed_out) { result.timed_out = true; return result; }
    result.root_upper = root.upper();
    if (verbose) std::cerr << "route_pool_lp_root_upper=" << result.root_upper << " route_pool_lp_root_pivots=" << root.pivots << '\n';
    bool integral = true;
    for (const auto& c : columns) integral &= c.profit == std::round(c.profit);
    std::vector<unsigned char> fixed(taxis,0), occupied(requests,0), forbidden(columns.size(),0);
    std::vector<int> selected(taxis,-1);
    const auto compatible = [&](std::size_t id) {
        const auto& c = columns[id];
        if (fixed[c.taxi] || forbidden[id]) return false;
        for (auto r : c.requests) if (occupied[r]) return false;
        return true;
    };
    std::function<void(const PackingLP&,double)> visit = [&](const PackingLP& lp, double profit) {
        if (result.timed_out) return;
        if (std::chrono::steady_clock::now() >= deadline) { result.timed_out = true; return; }
        ++result.nodes;
        double upper = lp.upper(); if (integral) upper = std::floor(upper);
        if (upper <= result.profit+1e-9) return;
        const auto x = lp.values();
        std::vector<std::size_t> ranking;
        for (std::size_t j = 0; j < columns.size(); ++j) if (compatible(j)) ranking.push_back(j);
        if (ranking.empty()) {
            if (profit > result.profit+1e-9) { result.profit = profit; result.selected = selected; }
            return;
        }
        std::stable_sort(ranking.begin(),ranking.end(),[&](auto a,auto b) {
            return x[a] != x[b] ? x[a] > x[b] : columns[a].profit > columns[b].profit;
        });
        auto used = occupied, assigned = fixed; auto fleet = selected;
        double candidate = profit;
        for (auto id : ranking) {
            const auto& c = columns[id];
            if (assigned[c.taxi]) continue;
            bool free = true; for (auto r : c.requests) free &= !used[r];
            if (!free) continue;
            assigned[c.taxi] = 1; for (auto r : c.requests) used[r] = 1;
            fleet[c.taxi] = static_cast<int>(id); candidate += c.profit;
        }
        if (candidate > result.profit+1e-9) {
            result.profit = candidate; result.selected = std::move(fleet);
            if (verbose) std::cerr << "route_pool_lp_incumbent=" << result.profit << " route_pool_lp_node=" << result.nodes << '\n';
        }
        if (upper <= result.profit+1e-9) return;
        std::size_t branch = ranking.front(); double fractionality = -1;
        for (auto j : ranking) {
            if (x[j] <= 1e-7 || x[j] >= 1-1e-7) continue;
            const auto score = std::min(x[j],1-x[j]);
            if (score > fractionality) { fractionality = score; branch = j; }
        }
        const auto& c = columns[branch];
        {
            auto child = lp; child.constrain(branch,true); child.dual(deadline);
            result.pivots += child.pivots;
            if (child.timed_out) { result.timed_out = true; return; }
            fixed[c.taxi] = 1; for (auto r : c.requests) occupied[r] = 1;
            selected[c.taxi] = static_cast<int>(branch); visit(child,profit+c.profit);
            selected[c.taxi] = -1;
            fixed[c.taxi] = 0; for (auto r : c.requests) occupied[r] = 0;
        }
        if (!result.timed_out) {
            auto child = lp; child.constrain(branch,false); child.dual(deadline);
            result.pivots += child.pivots;
            if (child.timed_out) { result.timed_out = true; return; }
            forbidden[branch] = 1; visit(child,profit); forbidden[branch] = 0;
        }
    };
    visit(root,0);
    result.complete = !result.timed_out;
    return result;
}

MasterResult improve_route_master(const std::vector<MasterColumn>& columns,
        std::size_t taxis, std::size_t requests, const std::vector<int>& incumbent,
        Deadline deadline, std::uint64_t seed, const std::vector<double>& request_prices) {
#ifdef SOICT_ROUTE_POOL_CPLEX
    try {return cplex_route_master(columns,taxis,requests,incumbent,deadline);}
    catch (const std::exception& error) {std::cerr<<"route_pool_cplex_error="<<error.what()<<'\n';}
#endif
    MasterResult result;result.selected = incumbent;
    result.root_upper = std::numeric_limits<double>::infinity();
    for (auto id : incumbent) if (id >= 0) result.profit += columns[id].profit;
    std::vector<std::vector<std::size_t>> banks(taxis);
    for (std::size_t j = 0; j < columns.size(); ++j) banks[columns[j].taxi].push_back(j);
    std::vector<double> penalties(columns.size(),0);
    if (!request_prices.empty()) for (std::size_t j=0;j<columns.size();++j)
        for (auto r:columns[j].requests) penalties[j]+=request_prices[r];
    // Price-based starts favor routes that leave expensive shared requests
    // available to other taxis. Each constructed fleet is checked directly.
    for (const double scale:{0.0,0.5,1.0,1.5}) {
        std::vector<std::size_t> ranking(columns.size());std::iota(ranking.begin(),ranking.end(),0);
        std::sort(ranking.begin(),ranking.end(),[&](auto a,auto b) {
            const auto x=columns[a].profit-scale*penalties[a],y=columns[b].profit-scale*penalties[b];
            return x!=y?x>y:a<b;
        });
        std::vector<unsigned char> occupied(requests,0);std::vector<int> fleet(taxis,-1);double profit=0;
        for (auto j:ranking) {
            const auto& c=columns[j];if (fleet[c.taxi]>=0) continue;
            bool free=true;for (auto r:c.requests) free&=!occupied[r];if (!free) continue;
            fleet[c.taxi]=static_cast<int>(j);for (auto r:c.requests) occupied[r]=1;profit+=c.profit;
        }
        if (profit>result.profit+1e-9) {result.profit=profit;result.selected=std::move(fleet);
            std::cerr<<"route_pool_block_incumbent="<<result.profit<<" route_pool_block_start_scale="<<scale<<'\n';}
    }
    for (auto& bank:banks) std::sort(bank.begin(),bank.end(),[&](auto a,auto b) {
        const auto x=columns[a].profit-penalties[a],y=columns[b].profit-penalties[b];
        return x!=y?x>y:a<b;
    });
    std::mt19937_64 rng(seed);
    const auto start = std::chrono::steady_clock::now();
    std::size_t rounds = 0, last_gain = 0;
    while (std::chrono::steady_clock::now() < deadline) {
        std::vector<int> owner(requests,-1);
        for (std::size_t t = 0; t < taxis; ++t) if (result.selected[t] >= 0)
            for (auto r : columns[result.selected[t]].requests) owner[r] = static_cast<int>(t);
        const std::size_t width = std::min(taxis,static_cast<std::size_t>(8+4*((rounds/taxis)%4)));
        std::vector<std::size_t> group;
        std::vector<unsigned char> inside(taxis,0);
        const auto add = [&](std::size_t t) { if (!inside[t]) { group.push_back(t);inside[t] = 1; } };
        const auto first = rounds%taxis; add(first);
        // Follow the owners blocking a promising alternative. Periodic
        // randomization diversifies the coordinated neighborhood.
        if (!banks[first].empty()) {
            const auto rank = rounds%3 == 0 ? rng()%std::min<std::size_t>(24,banks[first].size()) : 0;
            for (auto r : columns[banks[first][rank]].requests)
                if (owner[r] >= 0 && group.size() < width) add(owner[r]);
        }
        while (group.size() < width) {
            std::vector<double> score(taxis,0);
            for (auto t : group) {
                const auto old = result.selected[t] >= 0 ? columns[result.selected[t]].profit : 0;
                const auto count = std::min<std::size_t>(32,banks[t].size());
                for (std::size_t j = 0; j < count; ++j) {
                    const auto& c = columns[banks[t][j]];
                    const auto value = std::max(1.0,c.profit-old);
                    for (auto r : c.requests) if (owner[r] >= 0 && !inside[owner[r]]) score[owner[r]] += value;
                }
            }
            std::size_t chosen = taxis;double maximum = 0;
            for (std::size_t t = 0; t < taxis; ++t) if (!inside[t] && score[t] > maximum) {
                maximum = score[t]; chosen = t;
            }
            if (chosen == taxis || rounds%5 == 4) {
                do { chosen = rng()%taxis; } while (inside[chosen]);
            }
            add(chosen);
        }
        std::vector<MasterColumn> local;
        std::vector<std::size_t> global;
        std::vector<int> initial(group.size(),-1);
        for (std::size_t t = 0; t < group.size(); ++t) for (auto id : banks[group[t]]) {
            const auto& c = columns[id];bool available = true;
            for (auto r : c.requests) if (owner[r] >= 0 && !inside[owner[r]]) { available = false; break; }
            if (!available) continue;
            if (result.selected[group[t]] == static_cast<int>(id)) initial[t] = static_cast<int>(local.size());
            local.push_back({t,c.requests,c.profit});global.push_back(id);
        }
        const auto now = std::chrono::steady_clock::now();
        const auto local_deadline = std::min(deadline,now+std::chrono::milliseconds(150));
        const auto solved = solve_route_master(local,group.size(),requests,initial,local_deadline,false);
        result.nodes += solved.nodes;result.pivots += solved.pivots;
        double old = 0;for (auto id : initial) if (id >= 0) old += local[id].profit;
        if (solved.profit > old+1e-9) {
            for (std::size_t t = 0; t < group.size(); ++t)
                result.selected[group[t]] = solved.selected[t] >= 0 ? static_cast<int>(global[solved.selected[t]]) : -1;
            result.profit += solved.profit-old;last_gain = rounds;
            std::cerr << "route_pool_block_incumbent=" << result.profit << " route_pool_block_round=" << rounds
                      << " route_pool_block_seconds=" << std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count() << '\n';
        }
        ++rounds;
        // Return a useful incumbent instead of spending the entire allowance
        // on identical neighborhoods after several unsuccessful full sweeps.
        if (rounds-last_gain > 12*taxis) break;
    }
    std::cerr << "route_pool_block_rounds=" << rounds << '\n';
    result.timed_out = std::chrono::steady_clock::now()>=deadline;
    return result;
}
}
