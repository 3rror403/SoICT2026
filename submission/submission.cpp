#pragma GCC optimize("O3", "unroll-loops")
#include <bits/stdc++.h>
using namespace std;
using Clock=chrono::steady_clock;
using Sec=chrono::seconds;
using Ms=chrono::milliseconds;
using Ints=vector<int>;
using U64=uint64_t;
using Real=double;
using Reals=vector<Real>;
using Ids=vector<size_t>;
Real INF=numeric_limits<Real>::infinity();
const Real EPS=1e-8, GAIN_EPS=1e-9, PIVOT_EPS=1e-9, LP_EPS=1e-7, BOUND_REL_EPS=1e-10;

// --- Problem data ---
struct Req {
	int p, d;
	bool passenger;
	Real w, ready, late, dready, dlate, service, revenue;
};
struct Col {
	Ints route;
	Real gain;
};
struct Problem {
	int k, n, m, v, R;
	Ints origin;
	Reals cap, t, c;
	vector<Req> q;
	Ints who, drop;
};

// --- Input ---
Problem read() {
	Problem x;
	cin >> x.k >> x.n >> x.m;
	x.R=x.n + x.m;
	x.v=x.k + 2 * x.R;
	x.origin.resize(x.k);
	x.cap.resize(x.k);
	for (int i=0; i < x.k; i++) cin >> x.origin[i] >> x.cap[i];
	x.q.resize(x.R);
	x.who.assign(x.v + 1, -1);
	x.drop.assign(x.v + 1, 0);
	for (int i=0; i < x.n; i++) {
		auto &q=x.q[i];
		q.passenger=1;
		cin >> q.p >> q.d >> q.ready >> q.late >> q.service >> q.revenue;
		x.who[q.p]=x.who[q.d]=i;
		x.drop[q.d]=1;
	}
	for (int j=0; j < x.m; j++) {
		auto &q=x.q[x.n + j];
		q.passenger=0;
		cin >> q.p >> q.d >> q.w >> q.ready >> q.late >> q.dready >> q.dlate >> q.service >> q.revenue;
		x.who[q.p]=x.who[q.d]=x.n + j;
		x.drop[q.d]=1;
	}
	x.t.resize(x.v * x.v);
	x.c.resize(x.v * x.v);
	for (auto &z : x.t) cin >> z;
	for (auto &z : x.c) cin >> z;
	return x;
}

// --- Route feasibility and profit ---
optional<Real> eval(const Problem &x, int cab, const Ints &r) {
	static vector<U64> picked, delivered;
	static U64 stamp=0;
	if (picked.size() != (size_t)x.R) {
		picked.assign(x.R, 0);
		delivered.assign(x.R, 0);
	}
	++stamp;
	Real now=0, load=0, cost=0, revenue=0;
	int previous=x.origin[cab], open=0;
	for (int pos=0; pos < (int)r.size(); ++pos) {
		int node=r[pos], id=x.who[node];
		auto &q=x.q[id];
		Real arrival=now + x.t[(previous - 1) * x.v + node - 1];
		if (!isfinite(arrival)) return {};
		cost += x.c[(previous - 1) * x.v + node - 1];
		if (!x.drop[node]) {
			if (picked[id] == stamp || arrival > q.late + EPS) return {};
			if (q.passenger && (pos + 1 == (int)r.size() || r[pos + 1] != q.d)) return {};
			load += q.w;
			if (load > x.cap[cab] + EPS) return {};
			picked[id]=stamp;
			++open;
			revenue += q.revenue;
			now=max(arrival, q.ready) + q.service;
		} else {
			if (picked[id] != stamp || delivered[id] == stamp || (!q.passenger && arrival > q.dlate + EPS)) return {};
			load -= q.w;
			delivered[id]=stamp;
			--open;
			now=max(arrival, q.dready) + q.service;
		}
		previous=node;
	}
	if (open) return {};
	return revenue - cost;
}
using Deadline=Clock::time_point;
struct Column {
	size_t cab;
	Ids reqs;
	Real profit;
};
struct Result {
	Ints chosen;
	Real profit=0;
	bool timeout=false;
};

// --- Set-packing LP ---
struct Packing {
	const vector<Column> *cols;
	vector<Ids> rows;
	Ids labels;
	size_t dim=0, k;
};
Packing make_matrix(const vector<Column> &c, size_t k, size_t reqs) {
	Packing m;
	m.cols=&c;
	m.rows.resize(c.size());
	m.k=k;
	Ints owner(reqs, -1);
	for (auto &column : c)
		for (auto r : column.reqs) {
			if (owner[r] == -1) owner[r]=column.cab;
			else if (owner[r] != (int)column.cab) owner[r]=-2;
		}
	Ints index(k + reqs, -1);
	for (size_t j=0; j < c.size(); ++j) {
		auto add=[&](size_t r) {
			if (r >= k && owner[r - k] != -2) return;
			if (index[r] < 0) {
				index[r]=int(m.dim++);
				m.labels.push_back(r);
			}
			m.rows[j].push_back(index[r]);
		};
		add(c[j].cab);
		for (auto r : c[j].reqs) add(k + r);
	}
	return m;
}
struct LP {
	const Packing *mat;
	size_t n;
	Reals inv, basic, prices, rhs, signs;
	Ids basis;
	vector<unsigned char> active;
	Ints bound;
	bool timeout=false;
};
LP make_lp(const Packing &mat) {
	LP lp;
	lp.mat=&mat;
	lp.n=mat.dim;
	lp.inv.assign(lp.n * lp.n, 0);
	lp.basic.assign(lp.n, 1);
	lp.prices.assign(lp.n, 0);
	lp.rhs.assign(lp.n, 1);
	lp.signs.assign(lp.n, 0);
	lp.basis.resize(lp.n);
	lp.active.assign(mat.cols->size() + lp.n, 0);
	lp.bound.assign(mat.cols->size(), -1);
	for (size_t i=0; i < lp.n; ++i) {
		lp.inv[i * lp.n + i]=1;
		lp.basis[i]=mat.cols->size() + i;
		lp.active[lp.basis[i]]=1;
	}
	return lp;
}
Real coeff(const LP &lp, size_t row, size_t column) {
	auto nc=lp.mat->cols->size();
	if (column >= nc) return lp.inv[row * lp.n + column - nc];
	Real value=0;
	for (auto r : lp.mat->rows[column]) value += lp.inv[row * lp.n + r];
	if (lp.bound[column] >= 0) {
		auto r=size_t(lp.bound[column]);
		value += lp.signs[r] * lp.inv[row * lp.n + r];
	}
	return value;
}
Real reduced(const LP &lp, size_t column) {
	auto nc=lp.mat->cols->size();
	if (column >= nc) return -lp.prices[column - nc];
	Real value=(*lp.mat->cols)[column].profit;
	for (auto r : lp.mat->rows[column]) value -= lp.prices[r];
	if (lp.bound[column] >= 0) {
		auto r=size_t(lp.bound[column]);
		value -= lp.signs[r] * lp.prices[r];
	}
	return value;
}
void pivot(LP &lp, size_t leaving, size_t entering) {
	Reals dir(lp.n), row(lp.n);
	for (size_t i=0; i < lp.n; ++i) dir[i]=coeff(lp, i, entering);
	auto divisor=dir[leaving];
	Real step=lp.basic[leaving] / divisor, gain=reduced(lp, entering);
	for (size_t r=0; r < lp.n; ++r) {
		row[r]=lp.inv[leaving * lp.n + r] / divisor;
		lp.prices[r] += gain * row[r];
	}
	for (size_t i=0; i < lp.n; ++i)
		if (i != leaving) {
			lp.basic[i] -= step * dir[i];
			if (dir[i] == 0) continue;
			for (size_t r=0; r < lp.n; ++r) lp.inv[i * lp.n + r] -= dir[i] * row[r];
		}
	copy(row.begin(), row.end(), lp.inv.begin() + leaving * lp.n);
	lp.basic[leaving]=step;
	lp.active[lp.basis[leaving]]=0;
	lp.basis[leaving]=entering;
	lp.active[entering]=1;
}

// --- Primal and dual simplex ---
void primal(LP &lp, Deadline until) {
	auto nc=lp.mat->cols->size();
	size_t degenerate=0;
	for (size_t iteration=0; iteration < 10000; ++iteration) {
		if ((iteration & 7) == 0 && Clock::now() >= until) {
			lp.timeout=true;
			return;
		}
		size_t entering=nc + lp.n;
		Real gain=EPS;
		for (size_t j=0; j < nc + lp.n; ++j) {
			if (lp.active[j]) continue;
			Real value=reduced(lp, j);
			if (value > gain) {
				gain=value;
				entering=j;
				if (degenerate > lp.n) break;
			}
		}
		if (entering == nc + lp.n) return;
		size_t leaving=lp.n;
		Real step=INF;
		for (size_t i=0; i < lp.n; ++i) {
			auto d=coeff(lp, i, entering);
			if (d <= PIVOT_EPS) continue;
			auto ratio=max(0.0, lp.basic[i]) / d;
			if (ratio < step - PIVOT_EPS ||
				(abs(ratio - step) <= PIVOT_EPS && (leaving == lp.n || lp.basis[i] < lp.basis[leaving]))) {
				leaving=i;
				step=ratio;
			}
		}
		if (leaving == lp.n || !isfinite(step)) return;
		degenerate=step < PIVOT_EPS ? degenerate + 1 : 0;
		pivot(lp, leaving, entering);
	}
}
void constrain(LP &lp, size_t column, bool include) {
	Real sign=include ? -1 : 1, rhs=include ? -1 : 0;
	auto old=lp.n++;
	Reals enlarged(lp.n * lp.n, 0);
	for (size_t i=0; i < old; ++i) copy_n(lp.inv.begin() + i * old, old, enlarged.begin() + i * lp.n);
	Real value=0;
	for (size_t i=0; i < old; ++i)
		if (lp.basis[i] == column) {
			value=lp.basic[i];
			for (size_t r=0; r < old; ++r) enlarged[old * lp.n + r]=-sign * lp.inv[i * old + r];
			break;
		}
	enlarged[old * lp.n + old]=1;
	lp.inv=move(enlarged);
	lp.basic.push_back(rhs - sign * value);
	lp.prices.push_back(0);
	lp.rhs.push_back(rhs);
	lp.signs.push_back(sign);
	lp.basis.push_back(lp.mat->cols->size() + old);
	lp.active.push_back(1);
	lp.bound[column]=int(old);
}
void dual(LP &lp, Deadline until) {
	auto nc=lp.mat->cols->size();
	for (size_t iteration=0; iteration < 10000; ++iteration) {
		if ((iteration & 7) == 0 && Clock::now() >= until) {
			lp.timeout=true;
			return;
		}
		size_t leaving=lp.n;
		Real minimum=-LP_EPS;
		for (size_t i=0; i < lp.n; ++i)
			if (lp.basic[i] < minimum) {
				minimum=lp.basic[i];
				leaving=i;
			}
		if (leaving == lp.n) return;
		size_t entering=nc + lp.n;
		Real ratio=INF;
		for (size_t j=0; j < nc + lp.n; ++j) {
			if (lp.active[j]) continue;
			auto d=coeff(lp, leaving, j);
			if (d >= -PIVOT_EPS) continue;
			Real candidate=reduced(lp, j) / d;
			if (candidate < ratio - PIVOT_EPS) {
				entering=j;
				ratio=candidate;
			}
		}
		if (entering == nc + lp.n) return;
		pivot(lp, leaving, entering);
	}
}
Reals values(const LP &lp) {
	Reals x(lp.mat->cols->size(), 0);
	for (size_t i=0; i < lp.n; ++i)
		if (lp.basis[i] < x.size() && isfinite(lp.basic[i])) x[lp.basis[i]]=clamp(lp.basic[i], 0.0, 1.0);
	return x;
}
Real upper(const LP &lp) {
	auto prices=lp.prices;
	Real bound=0, magnitude=0;
	for (size_t r=0; r < lp.n; ++r) {
		if (!isfinite(prices[r])) return INF;
		prices[r]=max(0.0, prices[r]);
		bound += prices[r] * lp.rhs[r];
		magnitude += abs(prices[r] * lp.rhs[r]);
	}
	Reals residual(lp.mat->k, 0);
	for (size_t j=0; j < lp.mat->cols->size(); ++j) {
		auto &c=(*lp.mat->cols)[j];
		Real value=c.profit;
		for (auto r : lp.mat->rows[j]) value -= prices[r];
		if (lp.bound[j] >= 0) {
			auto r=size_t(lp.bound[j]);
			value -= lp.signs[r] * prices[r];
		}
		residual[c.cab]=max(residual[c.cab], value);
	}
	auto remainder=accumulate(residual.begin(), residual.end(), 0.0);
	bound += remainder;
	magnitude += remainder;
	if (!isfinite(bound) || !isfinite(magnitude)) return INF;
	return bound + LP_EPS + BOUND_REL_EPS * magnitude;
}
Reals lp_prices(const LP &lp, size_t reqs) {
	Reals result(reqs, 0);
	for (size_t row=0; row < lp.mat->labels.size(); ++row) {
		auto resource=lp.mat->labels[row];
		if (resource >= lp.mat->k && isfinite(lp.prices[row])) result[resource - lp.mat->k]=max(0.0, lp.prices[row]);
	}
	return result;
}

// --- Branch-and-bound route selection ---
Result solve_master(const vector<Column> &cols, size_t k, size_t reqs, const Ints &prior, Deadline until) {
	Result result;
	result.chosen=prior;
	for (auto id : prior)
		if (id >= 0) result.profit += cols[id].profit;
	if (cols.empty()) {
		return result;
	}
	auto mat=make_matrix(cols, k, reqs);
	auto root=make_lp(mat);
	primal(root, until);
	if (root.timeout) {
		result.timeout=true;
		return result;
	}
	bool integral=true;
	for (auto &c : cols) integral &= c.profit == round(c.profit);
	vector<unsigned char> fixed(k, 0), occupied(reqs, 0), forbidden(cols.size(), 0);
	Ints chosen(k, -1);
	auto compatible=[&](size_t id) {
		auto &c=cols[id];
		if (fixed[c.cab] || forbidden[id]) return false;
		for (auto r : c.reqs)
			if (occupied[r]) return false;
		return true;
	};
	function<void(const LP &, Real)> visit=[&](const LP &lp, Real profit) {
		if (result.timeout) return;
		if (Clock::now() >= until) {
			result.timeout=true;
			return;
		}
		Real upper=::upper(lp);
		if (integral) upper=floor(upper);
		if (upper <= result.profit + GAIN_EPS) return;
		auto x=values(lp);
		Ids ranking;
		for (size_t j=0; j < cols.size(); ++j)
			if (compatible(j)) ranking.push_back(j);
		if (ranking.empty()) {
			if (profit > result.profit + GAIN_EPS) {
				result.profit=profit;
				result.chosen=chosen;
			}
			return;
		}
		stable_sort(ranking.begin(), ranking.end(),
					[&](auto a, auto b) { return x[a] != x[b] ? x[a] > x[b] : cols[a].profit > cols[b].profit; });
		auto used=occupied, assigned=fixed;
		auto fleet=chosen;
		Real candidate=profit;
		for (auto id : ranking) {
			auto &c=cols[id];
			if (assigned[c.cab]) continue;
			bool free=true;
			for (auto r : c.reqs) free &= !used[r];
			if (!free) continue;
			assigned[c.cab]=1;
			for (auto r : c.reqs) used[r]=1;
			fleet[c.cab]=int(id);
			candidate += c.profit;
		}
		if (candidate > result.profit + GAIN_EPS) {
			result.profit=candidate;
			result.chosen=move(fleet);
		}
		if (upper <= result.profit + GAIN_EPS) return;
		size_t branch=ranking.front();
		Real fractionality=-1;
		for (auto j : ranking) {
			if (x[j] <= LP_EPS || x[j] >= 1 - LP_EPS) continue;
			auto score=min(x[j], 1 - x[j]);
			if (score > fractionality) {
				fractionality=score;
				branch=j;
			}
		}
		auto &c=cols[branch];
		{
			auto child=lp;
			constrain(child, branch, true);
			dual(child, until);
			if (child.timeout) {
				result.timeout=true;
				return;
			}
			fixed[c.cab]=1;
			for (auto r : c.reqs) occupied[r]=1;
			chosen[c.cab]=int(branch);
			visit(child, profit + c.profit);
			chosen[c.cab]=-1;
			fixed[c.cab]=0;
			for (auto r : c.reqs) occupied[r]=0;
		}
		if (!result.timeout) {
			auto child=lp;
			constrain(child, branch, false);
			dual(child, until);
			if (child.timeout) {
				result.timeout=true;
				return;
			}
			forbidden[branch]=1;
			visit(child, profit);
			forbidden[branch]=0;
		}
	};
	visit(root, 0);
	return result;
}

// --- Coordinated taxi-group selection ---
Result group_master(const vector<Column> &cols, size_t k, size_t reqs, const Ints &prior, Deadline until, U64 seed,
					const Reals &lp_prices) {
	Result result;
	result.chosen=prior;
	for (auto id : prior)
		if (id >= 0) result.profit += cols[id].profit;
	vector<Ids> banks(k);
	for (size_t j=0; j < cols.size(); ++j) banks[cols[j].cab].push_back(j);
	Reals penalties(cols.size(), 0);
	if (!lp_prices.empty())
		for (size_t j=0; j < cols.size(); ++j)
			for (auto r : cols[j].reqs) penalties[j] += lp_prices[r];
	for (Real scale : {0.0, 0.5, 1.0, 1.5}) {
		Ids ranking(cols.size());
		iota(ranking.begin(), ranking.end(), 0);
		sort(ranking.begin(), ranking.end(), [&](auto a, auto b) {
			auto x=cols[a].profit - scale * penalties[a], y=cols[b].profit - scale * penalties[b];
			return x != y ? x > y : a < b;
		});
		vector<unsigned char> occupied(reqs, 0);
		Ints fleet(k, -1);
		Real profit=0;
		for (auto j : ranking) {
			auto &c=cols[j];
			if (fleet[c.cab] >= 0) continue;
			bool free=true;
			for (auto r : c.reqs) free &= !occupied[r];
			if (!free) continue;
			fleet[c.cab]=int(j);
			for (auto r : c.reqs) occupied[r]=1;
			profit += c.profit;
		}
		if (profit > result.profit + GAIN_EPS) {
			result.profit=profit;
			result.chosen=move(fleet);
		}
	}
	for (auto &bank : banks)
		sort(bank.begin(), bank.end(), [&](auto a, auto b) {
			auto x=cols[a].profit - penalties[a], y=cols[b].profit - penalties[b];
			return x != y ? x > y : a < b;
		});
	mt19937_64 rng(seed);
	size_t rounds=0, last_gain=0;
	while (Clock::now() < until) {
		Ints owner(reqs, -1);
		for (size_t t=0; t < k; ++t)
			if (result.chosen[t] >= 0)
				for (auto r : cols[result.chosen[t]].reqs) owner[r]=int(t);
		size_t width=min(k, size_t(8 + 4 * ((rounds / k) % 4)));
		Ids group;
		vector<unsigned char> inside(k, 0);
		auto add=[&](size_t t) {
			if (!inside[t]) {
				group.push_back(t);
				inside[t]=1;
			}
		};
		auto first=rounds % k;
		add(first);
		if (!banks[first].empty()) {
			auto rank=rounds % 3 == 0 ? rng() % min<size_t>(24, banks[first].size()) : 0;
			for (auto r : cols[banks[first][rank]].reqs)
				if (owner[r] >= 0 && group.size() < width) add(owner[r]);
		}
		while (group.size() < width) {
			Reals score(k, 0);
			for (auto t : group) {
				auto old=result.chosen[t] >= 0 ? cols[result.chosen[t]].profit : 0;
				auto count=min<size_t>(32, banks[t].size());
				for (size_t j=0; j < count; ++j) {
					auto &c=cols[banks[t][j]];
					auto value=max(1.0, c.profit - old);
					for (auto r : c.reqs)
						if (owner[r] >= 0 && !inside[owner[r]]) score[owner[r]] += value;
				}
			}
			size_t chosen=k;
			Real maximum=0;
			for (size_t t=0; t < k; ++t)
				if (!inside[t] && score[t] > maximum) {
					maximum=score[t];
					chosen=t;
				}
			if (chosen == k || rounds % 5 == 4) {
				do {
					chosen=rng() % k;
				} while (inside[chosen]);
			}
			add(chosen);
		}
		vector<Column> local;
		Ids global;
		Ints initial(group.size(), -1);
		for (size_t t=0; t < group.size(); ++t) {
			size_t kept=0;
			for (auto id : banks[group[t]]) {
				auto &c=cols[id];
				bool available=true;
				for (auto r : c.reqs)
					if (owner[r] >= 0 && !inside[owner[r]]) {
						available=false;
						break;
					}
				if (!available) continue;
				if (kept++ >= 64 && result.chosen[group[t]] != int(id)) continue;
				if (result.chosen[group[t]] == int(id)) initial[t]=int(local.size());
				local.push_back({t, c.reqs, c.profit});
				global.push_back(id);
			}
		}
		auto now=Clock::now();
		auto local_deadline=min(until, now + Ms(150));
		auto solved=solve_master(local, group.size(), reqs, initial, local_deadline);
		Real old=0;
		for (auto id : initial)
			if (id >= 0) old += local[id].profit;
		if (solved.profit > old + GAIN_EPS) {
			for (size_t t=0; t < group.size(); ++t)
				result.chosen[group[t]]=solved.chosen[t] >= 0 ? int(global[solved.chosen[t]]) : -1;
			result.profit += solved.profit - old;
			last_gain=rounds;
		}
		++rounds;
		if (rounds - last_gain > 12 * k) break;
	}
	result.timeout=Clock::now() >= until;
	return result;
}

// --- Search state and route bank ---
Problem P;
U64 serial=1, seed=1;
const U64 SEED_STRIDE=0x9e3779b97f4a7c15ULL; // Phase seeds.
mt19937_64 rng;
using Route=Ints;
using Fleet=vector<Route>;
using Bank=map<Ints, Col>;
vector<Bank> bank;
Reals floor_gain;
int bank_limit=1000;
Deadline finish;
Deadline after(Real sec) { return Clock::now() + chrono::duration_cast<Clock::duration>(chrono::duration<Real>(sec)); }
Real travel(int a, int b) { return P.t[(a - 1) * P.v + b - 1]; }
Real expense(int a, int b) { return P.c[(a - 1) * P.v + b - 1]; }
Ints served(const Route &r) {
	Ints ids;
	for (int z : r)
		if (!P.drop[z]) ids.push_back(P.who[z]);
	sort(ids.begin(), ids.end());
	return ids;
}
Real cost(int t, const Route &r) {
	Real cost=0;
	int last=P.origin[t];
	for (int z : r) {
		cost += expense(last, z);
		last=z;
	}
	return cost;
}
Real profit(int t, const Route &r) {
	Real gain=-cost(t, r);
	for (int z : r)
		if (!P.drop[z]) gain += P.q[P.who[z]].revenue;
	return gain;
}
Real fleet_profit(const Fleet &f) {
	Real gain=0;
	for (int t=0; t < P.k; t++) gain += profit(t, f[t]);
	return gain;
}
void store(int t, const Route &r, bool cap=false, Real gain=-INF) {
	if (gain == -INF) gain=profit(t, r);
	if (gain <= EPS) return;
	auto &b=bank[t];
	if (cap && b.size() >= (size_t)bank_limit && gain < floor_gain[t]) return;
	auto ids=served(r);
	auto it=b.lower_bound(ids);
	if (it != b.end() && it->first == ids) {
		if (it->second.gain >= gain) return;
		it->second={r, gain};
	} else b.emplace_hint(it, ids, Col{r, gain});
	if (!cap) floor_gain[t]=-INF;
	if (cap && b.size() > (size_t)bank_limit) {
		auto worst=min_element(b.begin(), b.end(), [](auto &a, auto &b) { return a.second.gain < b.second.gain; });
		b.erase(worst);
		floor_gain[t]=min_element(b.begin(), b.end(), [](auto &a, auto &b) {
							return a.second.gain < b.second.gain;
						})->second.gain;
	}
}
void remember(const Fleet &f) {
	for (int t=0; t < P.k; t++) store(t, f[t]);
}

// --- Directed travel lower bounds ---
Reals drop_bound;
void build_lower_bounds(Deadline until) {
	int n=P.v;
	vector<Reals> landmarks;
	Reals spread(n, INF);
	vector<char> used(n);
	int landmark=0;
	auto stop=min(until, after(min(.6, .25 * chrono::duration<Real>(until - Clock::now()).count())));
	for (int pass=0; pass < min(8, n) && Clock::now() < stop; pass++) {
		Reals d(P.t.begin() + landmark * n, P.t.begin() + (landmark + 1) * n);
		vector<char> visited(n);
		d[landmark]=0;
		bool complete=true;
		for (int step=0; step < n; step++) {
			if (step % 64 == 0 && Clock::now() >= stop) {
				complete=false;
				break;
			}
			int v=-1;
			Real best=INF;
			for (int u=0; u < n; u++)
				if (!visited[u] && d[u] < best) {
					best=d[u];
					v=u;
				}
			if (v < 0) break;
			visited[v]=1;
			for (int u=0; u < n; u++)
				if (!visited[u]) d[u]=min(d[u], best + P.t[v * n + u]);
		}
		if (!complete) break;
		used[landmark]=1;
		for (int u=0; u < n; u++) spread[u]=min(spread[u], d[u]);
		landmarks.push_back(move(d));
		Real farthest=-1;
		for (int u=0; u < n; u++)
			if (!used[u] && spread[u] > farthest) {
				farthest=spread[u];
				landmark=u;
			}
	}
	drop_bound.assign(n * P.m, 0);
	for (int node=0; node < n; node++)
		for (int r=0; r < P.m; r++)
			for (auto &d : landmarks) {
				int drop=P.q[P.n + r].d - 1;
				Real guard=EPS + 64 * numeric_limits<Real>::epsilon() * n * max({1.0, d[drop], d[node]});
				auto &b=drop_bound[node * P.m + r];
				b=max(b, d[drop] - d[node] - guard);
			}
}

// --- Route generation: bounded DFS ---
vector<Ints> next_req;
Ints dfs_state, path, dfs_ids;
size_t visits, dfs_ceiling=1000000;
Real priced_floor=-INF;
Deadline dfs_until;
bool dfs_stop;
Reals dfs_prices;
map<Ints, pair<Real, Route>> priced_bank;
size_t dfs_calls=0;
void route_dfs(int cab, int last, Real now, Real cost, Real revenue, Real load, int open) {
	if (dfs_stop) return;
	if (++visits >= dfs_ceiling || (visits % 64 == 0 && Clock::now() >= dfs_until)) {
		dfs_stop=true;
		return;
	}
	for (int id : dfs_ids)
		if (dfs_state[id] == 1 && now + drop_bound[(last - 1) * P.m + id - P.n] > P.q[id].dlate + EPS) return;
	if (!open && revenue - cost > EPS) {
		if (dfs_prices.empty()) store(cab, path, true, revenue - cost);
		else {
			Real score=revenue - cost;
			for (int id : dfs_ids) score -= dfs_prices[id];
			if (score > EPS && (priced_bank.size() < (size_t)bank_limit || score >= priced_floor)) {
				auto ids=served(path);
				auto it=priced_bank.find(ids);
				if (it == priced_bank.end() || it->second.first < score) {
					priced_bank[ids]={score, path};
					if (priced_bank.size() > (size_t)bank_limit) {
						priced_bank.erase(min_element(priced_bank.begin(), priced_bank.end(), [](auto &a, auto &b) {
							return a.second.first < b.second.first;
						}));
						priced_floor=min_element(priced_bank.begin(), priced_bank.end(), [](auto &a, auto &b) {
										   return a.second.first < b.second.first;
									   })->second.first;
					}
				}
			}
		}
	}
	size_t count=next_req[last].size(), deliveries=dfs_ids.size();
	U64 hash=0;
	if (!dfs_prices.empty()) {
		auto &next=next_req[last];
		count=partition_point(next.begin(), next.end(),
								[&](int id) { return now <= P.q[id].late - travel(last, P.q[id].p) + EPS; }) -
				next.begin();
		hash=seed + SEED_STRIDE * (dfs_calls - 1) + cab * 0x94d049bb133111ebULL + last * 0xbf58476d1ce4e5b9ULL;
		hash=(hash ^ (hash >> 30)) * 0xbf58476d1ce4e5b9ULL;
		hash ^= hash >> 27;
	}
	size_t events=deliveries + count, offset=events && (hash & 1) ? hash % events : 0;
	for (size_t i=0; i < events; i++) {
		auto event=offset ? (i + offset) % events : i;
		if (event < deliveries) {
			int id=dfs_ids[event];
			if (dfs_state[id] != 1 || P.q[id].passenger) continue;
			auto &q=P.q[id];
			Real arrival=now + travel(last, q.d);
			if (arrival > q.dlate + EPS) continue;
			dfs_state[id]=2;
			path.push_back(q.d);
			route_dfs(cab, q.d, max(arrival, q.dready) + q.service, cost + expense(last, q.d), revenue, load - q.w,
					  open - 1);
			path.pop_back();
			dfs_state[id]=1;
			if (dfs_stop) return;
			continue;
		}
		int id=next_req[last][event - deliveries];
		auto &q=P.q[id];
		Real arc=travel(last, q.p);
		Real guard=EPS + 8 * numeric_limits<Real>::epsilon() * max({1.0, abs(q.late), abs(arc)});
		if (now > q.late - arc + guard) {
			if (dfs_prices.empty()) break;
			continue;
		}
		Real arrival=now + arc;
		if (arrival > q.late + EPS) continue;
		if (dfs_state[id] || load + q.w > P.cap[cab] + EPS) continue;
		Real dep=max(arrival, q.ready) + q.service, spent=cost + expense(last, q.p);
		dfs_state[id]=1;
		dfs_ids.push_back(id);
		path.push_back(q.p);
		if (q.passenger) {
			dfs_state[id]=2;
			path.push_back(q.d);
			route_dfs(cab, q.d, dep + travel(q.p, q.d) + q.service, spent + expense(q.p, q.d), revenue + q.revenue,
					  load, open);
			path.pop_back();
			dfs_state[id]=1;
		} else route_dfs(cab, q.p, dep, spent, revenue + q.revenue, load + q.w, open + 1);
		path.pop_back();
		dfs_ids.pop_back();
		dfs_state[id]=0;
		if (dfs_stop) return;
	}
}
bool enumerate_routes(Deadline until, const Reals &prices={}) {
	dfs_prices=prices;
	auto call=dfs_calls++;
	if (!prices.empty()) {
		rng.seed(seed + SEED_STRIDE * call);
		Real scale[]={1, .8, 1.2};
		for (auto &p : dfs_prices) p *= scale[call % 3];
	}
	if (next_req.empty()) {
		build_lower_bounds(until);
		next_req.resize(P.v + 1);
		for (int node=1; node <= P.v; node++) {
			auto &v=next_req[node];
			for (int r=0; r < P.R; r++)
				if (travel(node, P.q[r].p) <= P.q[r].late + EPS) v.push_back(r);
			stable_sort(v.begin(), v.end(), [&](int a, int b) {
				return P.q[a].late - travel(node, P.q[a].p) > P.q[b].late - travel(node, P.q[b].p);
			});
		}
	}
	bool complete=true;
	dfs_state.assign(P.R, 0);
	for (int t=0; t < P.k; t++) {
		auto now=Clock::now();
		if (now >= until) return false;
		auto taxi_until=now + (until - now) / (P.k - t);
		dfs_until=taxi_until;
		dfs_stop=false;
		visits=0;
		dfs_ceiling=1000000;
		path.clear();
		dfs_ids.clear();
		priced_bank.clear();
		priced_floor=-INF;
		if (prices.empty()) route_dfs(t, P.origin[t], 0, 0, 0, 0, 0);
		else {
			Ints first;
			for (int id : next_req[P.origin[t]])
				if (P.q[id].w <= P.cap[t] + EPS && travel(P.origin[t], P.q[id].p) <= P.q[id].late + EPS)
					first.push_back(id);
			shuffle(first.begin(), first.end(), rng);
			int left=first.size();
			for (int id : first) {
				now=Clock::now();
				if (now >= taxi_until || visits >= 1000000) {
					complete=false;
					break;
				}
				dfs_until=now + (taxi_until - now) / left;
				dfs_ceiling=visits + max<size_t>(1, (1000000 - visits) / left);
				dfs_stop=false;
				auto &q=P.q[id];
				Real a=travel(P.origin[t], q.p), dep=max(a, q.ready) + q.service, cost=expense(P.origin[t], q.p);
				dfs_ids.push_back(id);
				path.push_back(q.p);
				dfs_state[id]=1;
				if (q.passenger) {
					dfs_state[id]=2;
					path.push_back(q.d);
					route_dfs(t, q.d, dep + travel(q.p, q.d) + q.service, cost + expense(q.p, q.d), q.revenue, 0, 0);
					path.pop_back();
				} else route_dfs(t, q.p, dep, cost, q.revenue, q.w, 1);
				dfs_state[id]=0;
				dfs_ids.pop_back();
				path.pop_back();
				complete &= !dfs_stop;
				left--;
			}
		}
		complete &= !dfs_stop;
		for (auto &[ids, c] : priced_bank) store(t, c.second);
	}
	return complete;
}

// --- Fleet state and route updates ---
struct State {
	Fleet route;
	Ints owner;
	Reals gain;
	vector<U64> version;
	Real value=0;
};
State make_state(const Fleet &f) {
	State s;
	s.route=f;
	s.owner.assign(P.R, -1);
	s.gain.resize(P.k);
	s.version.resize(P.k);
	for (int t=0; t < P.k; t++) {
		for (int id : served(f[t])) s.owner[id]=t;
		s.gain[t]=profit(t, f[t]);
		s.value += s.gain[t];
		s.version[t]=serial++;
	}
	return s;
}
void set_route(State &s, int t, const Route &r) {
	for (int id : served(s.route[t])) s.owner[id]=-1;
	s.value -= s.gain[t];
	s.route[t]=r;
	s.gain[t]=profit(t, r);
	s.value += s.gain[t];
	for (int id : served(r)) s.owner[id]=t;
	s.version[t]=serial++;
}
void set_pair(State &s, int a, const Route &ra, int b, const Route &rb) {
	if (a == b) {
		set_route(s, a, rb);
		return;
	}
	for (int t : {a, b}) {
		for (int id : served(s.route[t])) s.owner[id]=-1;
		s.value -= s.gain[t];
	}
	s.route[a]=ra;
	s.route[b]=rb;
	for (int t : {a, b}) {
		s.gain[t]=profit(t, s.route[t]);
		s.value += s.gain[t];
		for (int id : served(s.route[t])) s.owner[id]=t;
		s.version[t]=serial++;
	}
}
bool boundary(const Route &r, int a) { return !a || P.drop[r[a - 1]] || !P.q[P.who[r[a - 1]]].passenger; }
Route without(const Route &r, int id) {
	Route v;
	for (int z : r)
		if (P.who[z] != id) v.push_back(z);
	return v;
}
Route inserted(Route r, int id, int a, int b) {
	r.insert(r.begin() + b, P.q[id].d);
	r.insert(r.begin() + a, P.q[id].p);
	return r;
}
struct Choice {
	int id=-1, t=-1, a=0, b=0;
	Real gain=-INF;
};

// --- Cached route segments ---
struct Segment {
	int first=0, last=0;
	Real duration=0, ready=-INF, late=INF, load=0, low=0, high=0;
	bool good=true;
};
Segment node_segment(int node) {
	auto &q=P.q[P.who[node]];
	bool drop=P.drop[node];
	Segment s;
	s.first=s.last=node;
	s.duration=q.service;
	s.ready=(!drop ? q.ready : q.dready) + q.service;
	s.late=(!drop ? q.late : q.dlate) + EPS;
	if (drop && q.passenger) {
		s.ready=-INF;
		s.late=numeric_limits<Real>::max();
	}
	s.load=drop ? -q.w : q.w;
	s.low=min(0.0, s.load);
	s.high=max(0.0, s.load);
	return s;
}
Segment join(Segment a, const Segment &b) {
	if (!a.first) return b;
	if (!b.first) return a;
	Real arc=travel(a.last, b.first), duration=a.duration, ready=a.ready, load=a.load;
	a.good=a.good && b.good && ready + arc <= b.late;
	a.late=min(a.late, b.late - arc - duration);
	a.duration=duration + arc + b.duration;
	a.ready=max(ready + arc + b.duration, b.ready);
	a.low=min(a.low, load + b.low);
	a.high=max(a.high, load + b.high);
	a.load=load + b.load;
	a.last=b.last;
	return a;
}
bool feasible(int t, const Segment &s) {
	return !s.first || (s.good && travel(P.origin[t], s.first) <= s.late && s.low >= -EPS && s.high <= P.cap[t] + EPS);
}
struct RangeCache {
	Route nodes;
	vector<Segment> single;
	vector<vector<Segment>> table;
};
vector<RangeCache> range_cache;
RangeCache &ranges(int t, const Route &r) {
	if (range_cache.empty()) range_cache.resize(P.k);
	auto &c=range_cache[t];
	if (c.nodes != r) {
		c.nodes=r;
		c.single.clear();
		c.table.clear();
		int n=r.size();
		for (int node : r) c.single.push_back(node_segment(node));
		for (int half=1; half < n; half *= 2) {
			c.table.emplace_back(n);
			auto &level=c.table.back();
			for (int start=0; start < n; start += 2 * half) {
				int mid=min(start + half, n), end=min(start + 2 * half, n);
				if (mid > start) {
					level[mid - 1]=c.single[mid - 1];
					for (int i=mid - 1; i > start; i--) level[i - 1]=join(c.single[i - 1], level[i]);
				}
				if (mid < end) {
					level[mid]=c.single[mid];
					for (int i=mid + 1; i < end; i++) level[i]=join(level[i - 1], c.single[i]);
				}
			}
		}
	}
	return c;
}
Segment piece(const RangeCache &c, int a, int b) {
	if (a >= b) return {};
	if (a + 1 == b) return c.single[a];
	int level=31 - __builtin_clz((unsigned)(a ^ (b - 1)));
	return join(c.table[level][a], c.table[level][b - 1]);
}

// --- Insertion search and caching ---
Choice insertion(int id, int t, const Route &r, Real minimum=EPS) {
	Choice best;
	auto &q=P.q[id];
	if (q.w > P.cap[t] + EPS) return best;
	int len=r.size();
	static Reals pd, dd, adj, suffix;
	pd.resize(len + 1);
	dd.resize(len + 1);
	adj.resize(len + 1);
	suffix.assign(len + 2, INF);
	Real least=INF, pickup_floor=INF;
	for (int a=0; a <= len; a++) {
		int prev=a ? r[a - 1] : P.origin[t];
		Real old=a < len ? expense(prev, r[a]) : 0;
		pd[a]=expense(prev, q.p) - old + (a < len ? expense(q.p, r[a]) : 0);
		dd[a]=expense(prev, q.d) - old + (a < len ? expense(q.d, r[a]) : 0);
		adj[a]=expense(prev, q.p) + expense(q.p, q.d) - old + (a < len ? expense(q.d, r[a]) : 0);
		least=min(least, adj[a]);
		if (!q.passenger) {
			least=min(least, pickup_floor + dd[a]);
			pickup_floor=min(pickup_floor, pd[a]);
		}
	}
	if (q.revenue - least + EPS < minimum) return best;
	for (int a=len; a >= 0; a--) suffix[a]=min(dd[a], suffix[a + 1]);
	auto &c=ranges(t, r);
	auto pickup=node_segment(q.p), delivery=node_segment(q.d);
	for (int a=0; a <= len; a++)
		if (boundary(r, a)) {
			Real floor=max(minimum, best.gain), delta=q.passenger ? adj[a] : min(adj[a], pd[a] + suffix[a + 1]);
			if (q.revenue - delta + EPS < floor) continue;
			auto prefix=join(piece(c, 0, a), pickup);
			if (!feasible(t, prefix)) continue;
			for (int b=a; b <= (q.passenger ? a : len); b++)
				if (boundary(r, b)) {
					Real gain=q.revenue - (a == b ? adj[a] : pd[a] + dd[b]);
					if (gain < max(minimum, best.gain) - EPS) continue;
					auto complete=join(join(join(prefix, piece(c, a, b)), delivery), piece(c, b, len));
					if (feasible(t, complete) && (best.t < 0 || gain > best.gain)) best={id, t, a, b, gain};
				}
		}
	return best;
}
struct InsertCache {
	vector<vector<U64>> version;
	vector<vector<Choice>> choice;
};
Choice best_insertion(State &s, int id, InsertCache &cache) {
	if (cache.version.empty()) {
		cache.version.assign(P.R, vector<U64>(P.k));
		cache.choice.assign(P.R, vector<Choice>(P.k));
	}
	Choice best;
	for (int t=0; t < P.k; t++) {
		if (cache.version[id][t] != s.version[t]) {
			cache.choice[id][t]=insertion(id, t, s.route[t]);
			cache.version[id][t]=s.version[t];
		}
		auto q=cache.choice[id][t];
		if (q.t >= 0 && q.gain > EPS && q.gain > best.gain) best=q;
	}
	return best;
}
void apply_insert(State &s, Choice c) {
	set_route(s, c.t, inserted(s.route[c.t], c.id, c.a, c.b));
	store(c.t, s.route[c.t]);
}

// --- Local-search neighborhoods ---
bool remove_move(State &s) {
	Real best=EPS;
	int br=-1;
	Route route;
	for (int id=0; id < P.R; id++)
		if (s.owner[id] >= 0) {
			int t=s.owner[id];
			auto r=without(s.route[t], id);
			auto v=eval(P, t, r);
			if (v && *v - s.gain[t] > best) {
				best=*v - s.gain[t];
				br=id;
				route=move(r);
			}
		}
	if (br < 0) return false;
	set_route(s, s.owner[br], route);
	return true;
}
bool add_move(State &s, InsertCache &cache) {
	Choice best;
	for (int id=0; id < P.R; id++)
		if (s.owner[id] < 0) {
			auto c=best_insertion(s, id, cache);
			if (c.t >= 0 && c.gain > best.gain) best=c;
		}
	if (best.t < 0) return false;
	apply_insert(s, best);
	return true;
}
struct Replacement {
	int out=-1;
	Choice in;
	Real gain=-INF;
};
struct ReplaceCache {
	vector<U64> version;
	vector<char> free;
	vector<vector<Replacement>> row;
};
ReplaceCache replace_cache;
bool replace_move(State &s, Deadline until) {
	auto &cache=replace_cache;
	if (cache.version.empty()) {
		cache.version.assign(P.k, 0);
		cache.free.assign(P.R, 0);
		cache.row.assign(P.k, vector<Replacement>(P.R));
	}
	Ints incoming, new_free;
	for (int id=0; id < P.R; id++)
		if (s.owner[id] < 0) {
			incoming.push_back(id);
			if (!cache.free[id]) new_free.push_back(id);
		}
	for (int t=0; t < P.k; t++) {
		bool changed=cache.version[t] != s.version[t];
		auto &ids=changed ? incoming : new_free;
		if (ids.empty()) continue;
		cache.version[t]=0; // Invalidate before a timed refresh.
		for (int id : ids) cache.row[t][id]=Replacement{};
		for (int out : served(s.route[t])) {
			auto reduced=without(s.route[t], out);
			Real base=profit(t, reduced) - s.gain[t];
			for (int in : ids) {
				if (Clock::now() >= until) {
					fill(cache.version.begin(), cache.version.end(), 0);
					return false;
				}
				auto &entry=cache.row[t][in];
				Real floor=max(EPS, entry.gain);
				auto c=insertion(in, t, reduced, floor - base);
				if (c.t >= 0 && base + c.gain > floor) entry={out, c, base + c.gain};
			}
		}
		cache.version[t]=s.version[t];
	}
	for (int id=0; id < P.R; id++) cache.free[id]=s.owner[id] < 0;
	Replacement best;
	best.gain=EPS;
	for (int t=0; t < P.k; t++)
		for (int in : incoming) {
			auto &r=cache.row[t][in];
			if (r.out >= 0 &&
				(r.gain > best.gain || (r.gain == best.gain && tie(r.out, r.in.id, r.in.a, r.in.b) <
																   tie(best.out, best.in.id, best.in.a, best.in.b))))
				best=r;
		}
	if (best.out < 0) return false;
	int t=best.in.t;
	set_route(s, t, inserted(without(s.route[t], best.out), best.in.id, best.in.a, best.in.b));
	return true;
}
Ints clean_cuts(const Route &r) {
	Ints cuts{0};
	int open=0;
	for (int i=0; i < (int)r.size(); i++) {
		open += P.drop[r[i]] ? -1 : 1;
		if (!open) cuts.push_back(i + 1);
	}
	return cuts;
}
bool cut_move(State &s, Deadline until) {
	vector<Ints> cuts(P.k);
	for (int t=0; t < P.k; t++) cuts[t]=clean_cuts(s.route[t]);
	for (int a=0; a < P.k && Clock::now() < until; a++)
		for (int b=a + 1; b < P.k; b++) {
			auto &x=s.route[a];
			auto &y=s.route[b];
			int nx=x.size(), ny=y.size();
			if (x.empty() && y.empty()) continue;
			Real best=EPS;
			int ca=-1, cb=-1;
			auto &sx=ranges(a, x);
			auto &sy=ranges(b, y);
			for (int i : cuts[a])
				for (int j : cuts[b]) {
					int pa=i ? x[i - 1] : P.origin[a], pb=j ? y[j - 1] : P.origin[b];
					Real gain=0;
					if (i < nx) gain += expense(pa, x[i]) - expense(pb, x[i]);
					if (j < ny) gain += expense(pb, y[j]) - expense(pa, y[j]);
					if (gain <= best + EPS) continue;
					if (!feasible(a, join(piece(sx, 0, i), piece(sy, j, ny))) ||
						!feasible(b, join(piece(sy, 0, j), piece(sx, i, nx))))
						continue;
					best=gain;
					ca=i;
					cb=j;
				}
			if (ca >= 0) {
				Route u(x.begin(), x.begin() + ca), v(y.begin(), y.begin() + cb);
				u.insert(u.end(), y.begin() + cb, y.end());
				v.insert(v.end(), x.begin() + ca, x.end());
				set_pair(s, a, u, b, v);
				return true;
			}
		}
	return false;
}
bool relocate_move(State &s, Deadline until) {
	Real best=EPS;
	int from=-1, to=-1;
	Route a, b;
	for (int id=0; id < P.R && Clock::now() < until; id++)
		if (s.owner[id] >= 0) {
			int t=s.owner[id];
			auto reduced=without(s.route[t], id);
			auto valid=eval(P, t, reduced);
			Real base=profit(t, reduced) - s.gain[t];
			for (int u=0; u < P.k; u++) {
				if (u != t && !valid) continue;
				auto &r=u == t ? reduced : s.route[u];
				auto c=insertion(id, u, r, best - base);
				if (c.t >= 0 && base + c.gain > best) {
					best=base + c.gain;
					from=t;
					to=u;
					a=reduced;
					b=inserted(r, id, c.a, c.b);
				}
			}
		}
	if (from < 0) return false;
	set_pair(s, from, a, to, b);
	return true;
}
bool swap_move(State &s, Deadline until) {
	Real best=EPS;
	int ta=-1, tb=-1;
	Route ra, rb;
	for (int id=0; id < P.R && Clock::now() < until; id++)
		if (s.owner[id] >= 0)
			for (int j=id + 1; j < P.R; j++)
				if (s.owner[j] >= 0 && s.owner[id] != s.owner[j]) {
					int a=s.owner[id], b=s.owner[j];
					auto replace=[&](int t, int out, int in) {
						auto r=s.route[t];
						int p=find(r.begin(), r.end(), P.q[out].p) - r.begin(),
							d=find(r.begin(), r.end(), P.q[out].d) - r.begin();
						r.erase(r.begin() + d);
						r.erase(r.begin() + p);
						return inserted(r, in, p, P.q[in].passenger ? p : d - 1);
					};
					auto u=replace(a, id, j), v=replace(b, j, id);
					Real gain=profit(a, u) + profit(b, v) - s.gain[a] - s.gain[b];
					if (gain <= best) continue;
					if (eval(P, a, u) && eval(P, b, v)) {
						best=gain;
						ta=a;
						tb=b;
						ra=move(u);
						rb=move(v);
					}
				}
	if (ta < 0) return false;
	set_pair(s, ta, ra, tb, rb);
	return true;
}

// --- Local-search descent ---
void polish(State &s, Deadline until, bool full, int cycles=3) {
	static InsertCache cache;
	for (int cycle=0; cycle < cycles && Clock::now() < until; cycle++) {
		bool changed=false;
		while (Clock::now() < until && remove_move(s)) changed=true;
		while (Clock::now() < until && add_move(s, cache)) changed=true;
		if (full)
			while (Clock::now() < until && relocate_move(s, until)) changed=true;
		while (Clock::now() < until && cut_move(s, until)) changed=true;
		if (full)
			while (Clock::now() < until && swap_move(s, until)) changed=true;
		while (Clock::now() < until && replace_move(s, until)) changed=true;
		if (!changed) break;
	}
	remember(s.route);
}

// --- Fleet destruction ---
void perturb(State &s, int round) {
	int count=max(4, (int)count_if(s.owner.begin(), s.owner.end(), [](int t) { return t >= 0; }) / 12);
	if (round % 4 == 0) {
		for (int i=0, n=2 + rng() % max(2, P.k / 12); i < n; i++) {
			int t=rng() % P.k;
			if (!s.route[t].empty()) set_route(s, t, {});
		}
		return;
	}
	Ints ids;
	for (int r=0; r < P.R; r++)
		if (s.owner[r] >= 0) ids.push_back(r);
	if (ids.empty()) return;
	if (round % 4 == 1) {
		ids.resize(P.R);
		iota(ids.begin(), ids.end(), 0);
		shuffle(ids.begin(), ids.end(), rng);
		int removed=0;
		for (int id : ids)
			if (s.owner[id] >= 0) {
				int t=s.owner[id];
				auto r=without(s.route[t], id);
				if (eval(P, t, r)) {
					set_route(s, t, r);
					if (++removed == count) break;
				}
			}
		return;
	}
	int anchor=round % 4 == 2 ? ids[rng() % ids.size()] : 0;
	for (int removed=0; removed < count; removed++) {
		int chosen=-1;
		Real best=-INF;
		Route route;
		for (int id=0; id < P.R; id++)
			if (s.owner[id] >= 0) {
				int t=s.owner[id];
				auto r=without(s.route[t], id);
				auto profit=eval(P, t, r);
				if (!profit) continue;
				Real score=*profit - s.gain[t];
				if (round % 4 == 2)
					score=-travel(P.q[anchor].p, P.q[id].p) - travel(P.q[id].p, P.q[anchor].p) -
							abs(P.q[anchor].ready - P.q[id].ready);
				if (score > best) {
					best=score;
					chosen=id;
					route=move(r);
				}
			}
		if (chosen < 0) break;
		set_route(s, s.owner[chosen], route);
	}
}

// --- Route generation: fleet rebuilding ---
size_t fleet_calls=0;
void sample(State &start, Deadline until) {
	rng.seed(seed + SEED_STRIDE * fleet_calls++);
	State state=start, best=start;
	InsertCache cache;
	Ints order(P.R);
	iota(order.begin(), order.end(), 0);
	int round=0;
	while (Clock::now() < until) {
		if (round) {
			if (round % 8 == 0 || state.value < best.value * .995) state=best;
			perturb(state, round);
		}
		shuffle(order.begin(), order.end(), rng);
		for (int id : order) {
			if (Clock::now() >= until) break;
			if (state.owner[id] >= 0) continue;
			auto c=best_insertion(state, id, cache);
			if (c.t >= 0) apply_insert(state, c);
		}
		while (Clock::now() < until && add_move(state, cache)) {
		}
		if (round % 2 == 0 && Clock::now() < until) polish(state, until, false);
		if (state.value > best.value) {
			best=state;
		}
		round++;
	}
	start=best;
	remember(start.route);
}

// --- Request-price estimation ---
Reals estimate_prices(Real feasible=0) {
	Reals prices(P.R), best_prices(P.R);
	Real lower=0, best=INF;
	vector<Column> cols;
	for (int t=0; t < P.k; t++)
		for (auto &[ids, c] : bank[t]) cols.push_back({(size_t)t, Ids(ids.begin(), ids.end()), c.gain});
	Ints ord(cols.size());
	iota(ord.begin(), ord.end(), 0);
	sort(ord.begin(), ord.end(), [&](int a, int b) { return cols[a].profit > cols[b].profit; });
	vector<char> used(P.R), assigned(P.k);
	for (int j : ord) {
		auto &c=cols[j];
		if (assigned[c.cab]) continue;
		bool ok=true;
		for (int r : c.reqs) ok &= !used[r];
		if (ok) {
			lower += c.profit;
			assigned[c.cab]=1;
			for (int r : c.reqs) used[r]=1;
		}
	}
	lower=max(lower, feasible);
	for (int it=0; it < 192; it++) {
		Reals mx(P.k);
		Ints chosen(P.k, -1), uses(P.R);
		for (int j=0; j < (int)cols.size(); j++) {
			auto &c=cols[j];
			Real v=c.profit;
			for (int r : c.reqs) v -= prices[r];
			if (v > mx[c.cab]) mx[c.cab]=v, chosen[c.cab]=j;
		}
		Real dual=accumulate(prices.begin(), prices.end(), 0.0) + accumulate(mx.begin(), mx.end(), 0.0);
		if (dual < best) best=dual, best_prices=prices;
		for (int j : chosen)
			if (j >= 0)
				for (int r : cols[j].reqs) uses[r]++;
		Real norm=0;
		for (int r=0; r < P.R; r++)
			if (prices[r] > 0 || uses[r] > 1) norm += (uses[r] - 1) * (uses[r] - 1);
		if (norm == 0 || dual <= lower + LP_EPS) break;
		Real step=.7 * (dual - lower) / norm;
		for (int r=0; r < P.R; r++) prices[r]=max(0.0, prices[r] + step * (uses[r] - 1));
	}
	return best_prices;
}

// --- Price-guided route selection ---
Result ranked_master(const vector<Column> &cols, int k, int reqs, const Ints &prior, Deadline until) {
	Result best;
	best.chosen=prior;
	for (int j : prior)
		if (j >= 0) best.profit += cols[j].profit;
	Reals prices;
	auto mat=make_matrix(cols, k, reqs);
	auto lp=make_lp(mat);
	primal(lp, min(until, after(6)));
	prices=lp_prices(lp, reqs);
	vector<Ints> banks(k);
	Reals score(cols.size());
	for (int j=0; j < (int)cols.size(); j++) {
		auto &c=cols[j];
		banks[c.cab].push_back(j);
		score[j]=c.profit;
		for (int r : c.reqs) score[j] -= prices[r];
	}
	for (auto &b : banks)
		sort(b.begin(), b.end(), [&](int a, int b) { return score[a] != score[b] ? score[a] > score[b] : a < b; });
	for (int width : {32, 64, 128}) {
		if (Clock::now() >= until) break;
		vector<Column> subset;
		Ints global, initial(k, -1);
		for (int t=0; t < k; t++) {
			auto add=[&](int j) {
				if (best.chosen[t] == j) initial[t]=subset.size();
				subset.push_back(cols[j]);
				global.push_back(j);
			};
			for (int i=0; i < min<int>(width, banks[t].size()); i++) add(banks[t][i]);
			if (best.chosen[t] >= 0 && initial[t] < 0) add(best.chosen[t]);
		}
		auto solved=solve_master(subset, k, reqs, initial, min(until, after(width == 32 ? 4 : 6)));
		if (solved.profit > best.profit + EPS) {
			best.profit=solved.profit;
			for (int t=0; t < k; t++) best.chosen[t]=solved.chosen[t] >= 0 ? global[solved.chosen[t]] : -1;
		}
	}
	if (Clock::now() + Ms(100) < until) {
		auto grouped=group_master(cols, k, reqs, best.chosen, until, rng(), prices);
		if (grouped.profit > best.profit + EPS) {
			best.profit=grouped.profit;
			best.chosen=move(grouped.chosen);
		}
	}
	return best;
}
void select_routes(State &s, Deadline until) {
	remember(s.route);
	vector<Column> cols;
	vector<Route> routes;
	Ints initial(P.k, -1);
	for (int t=0; t < P.k; t++)
		for (auto &[ids, c] : bank[t]) {
			if (ids == served(s.route[t])) initial[t]=cols.size();
			cols.push_back({(size_t)t, Ids(ids.begin(), ids.end()), c.gain});
			routes.push_back(c.route);
		}
	auto prices=P.k > 48 ? estimate_prices(s.value) : Reals{};
	auto solved=P.k > 64 ? group_master(cols, P.k, P.R, initial, until, seed, prices)
						   : (P.k > 48 ? ranked_master(cols, P.k, P.R, initial, until)
									   : solve_master(cols, P.k, P.R, initial, until));
	Fleet f(P.k);
	for (int t=0; t < P.k; t++)
		if (solved.chosen[t] >= 0) f[t]=routes[solved.chosen[t]];
	if (fleet_profit(f) > s.value + EPS) {
		s=make_state(f);
	}
}

// --- Route generation: priced replacements ---
size_t price_calls=0;
void priced_changes(State &s, Deadline until) {
	auto prices=estimate_prices(s.value);
	auto call=price_calls++;
	rng.seed(seed + SEED_STRIDE * call);
	Real scale[]={1, .8, 1.2};
	for (auto &p : prices) p *= scale[call % 3];
	for (int t=0; t < P.k && Clock::now() < until; t++) {
		auto now=Clock::now(), taxi_until=now + (until - now) / (P.k - t);
		vector<pair<Real, const Col *>> ranked;
		for (auto &[ids, c] : bank[t]) {
			Real z=c.gain;
			for (int id : ids) z -= prices[id];
			ranked.push_back({z, &c});
		}
		int count=min<int>(3, ranked.size());
		partial_sort(ranked.begin(), ranked.begin() + count, ranked.end(),
					 [](auto &a, auto &b) { return a.first > b.first; });
		vector<Route> seeds{s.route[t]};
		for (int i=0; i < count; i++) seeds.push_back(ranked[i].second->route);
		State one=make_state(Fleet(P.k));
		set_route(one, t, seeds[0]);
		int restarts=0;
		for (int it=0; it < 48 && Clock::now() < taxi_until; it++) {
			Real best=EPS;
			Route chosen;
			bool changed=false;
			auto score=[&](const Route &r) {
				Real z=profit(t, r);
				for (int id : served(r)) z -= prices[id];
				return z;
			};
			Real old=score(one.route[t]);
			for (int id=0; id < P.R && Clock::now() < taxi_until; id++)
				if (one.owner[id] < 0) {
					auto c=insertion(id, t, one.route[t], best + prices[id]);
					if (c.t >= 0 && c.gain - prices[id] > best) {
						best=c.gain - prices[id];
						chosen=inserted(one.route[t], id, c.a, c.b);
						changed=true;
					}
				}
			for (int out : served(one.route[t])) {
				auto r=without(one.route[t], out);
				Real base=profit(t, r) - one.gain[t];
				if (eval(P, t, r) && score(r) - old > best) {
					best=score(r) - old;
					chosen=r;
					changed=true;
				}
				for (int in=0; in < P.R && Clock::now() < taxi_until; in++)
					if (one.owner[in] < 0) {
						auto c=insertion(in, t, r, best - base - prices[out] + prices[in]);
						if (c.t >= 0 && base + c.gain + prices[out] - prices[in] > best) {
							best=base + c.gain + prices[out] - prices[in];
							chosen=inserted(r, in, c.a, c.b);
							changed=true;
						}
					}
			}
			if (!changed) {
				if (++restarts > 8) break;
				set_route(one, t, seeds[restarts % seeds.size()]);
				auto ids=served(one.route[t]);
				if (!ids.empty()) {
					shuffle(ids.begin(), ids.end(), rng);
					ids.resize(min<size_t>(ids.size(), 1 + rng() % 3));
					Route reduced;
					for (int node : one.route[t])
						if (find(ids.begin(), ids.end(), P.who[node]) == ids.end()) reduced.push_back(node);
					if (eval(P, t, reduced)) set_route(one, t, reduced);
				}
				continue;
			}
			set_route(one, t, chosen);
			store(t, chosen);
		}
	}
}

// --- Search pipeline ---
int main(int argc, char **argv) {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);
	auto started=Clock::now();
	if (argc > 1) seed=strtoull(argv[1], nullptr, 10);
	rng.seed(seed);
	P=read();
	finish=started + Sec(299);
	if (P.k > 96) bank_limit=400;
	bank.resize(P.k);
	floor_gain.assign(P.k, -INF);
	State state=make_state(Fleet(P.k));
	// Initial enumeration.
	bool complete=enumerate_routes(min(finish, after(P.k > 96 ? 2 : 40)));
	if (!complete || P.k > 48) {
		int phase=0;
		if (P.k > 48) select_routes(state, min(finish, after(20)));
		// Alternate fleet rebuilding, selection, and pricing.
		auto generating=P.k > 64 ? finish - Sec(8) : min(finish - Sec(4), after(100));
		do {
			sample(state, P.k > 48 ? min(generating, after(6)) : generating);
			if (P.k > 48 && Clock::now() + Sec(2) < generating) {
				select_routes(state, min(generating, after(6)));
				if (Clock::now() < generating) {
					if (++phase % 2) priced_changes(state, min(generating, after(3)));
					else enumerate_routes(min(generating, after(3)), estimate_prices(state.value));
				}
			}
		} while (P.k > 48 && Clock::now() < generating);
	}
	// Final selection and polishing.
	select_routes(state, min(finish - Sec(2), after(40)));
	if (Clock::now() < finish) polish(state, finish, true, 2);
	// Output the complete solution.
	Real value=0;
	for (int t=0; t < P.k; t++) value -= cost(t, state.route[t]);
	for (int r=0; r < P.R; r++)
		if (state.owner[r] >= 0) value += P.q[r].revenue;
	cout << setprecision(17) << value << '\n';
	for (auto &r : state.route) {
		cout << r.size();
		for (int z : r) cout << ' ' << z;
		cout << '\n';
	}
	return 0;
}
