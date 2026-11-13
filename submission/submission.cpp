// Passenger-parcel routing: generate routes, select a compatible fleet, then improve it.
// Layout: settings/data -> declarations -> pipeline/search -> helper implementations.
// Node IDs are one-based; taxi and request indices are zero-based. Tabs use width 4.
#pragma GCC optimize("O3", "unroll-loops")
#include <bits/stdc++.h>
using namespace std;

using Clock = chrono::steady_clock;
using Seconds = chrono::seconds;
using Milliseconds = chrono::milliseconds;
using IntVector = vector<int>;
using UInt64 = uint64_t;
using Real = double;
using RealVector = vector<Real>;
using IndexVector = vector<size_t>;
using Deadline = Clock::time_point;
using Route = IntVector;
using Fleet = vector<Route>;

// --- Settings: numerical tolerances and random streams ---
Real INF = numeric_limits<Real>::infinity();
const Real FEASIBILITY_EPSILON = 1e-8, IMPROVEMENT_EPSILON = 1e-9, PIVOT_EPSILON = 1e-9,
		   LP_FEASIBILITY_EPSILON = 1e-7, BOUND_RELATIVE_EPSILON = 1e-10;
// Fixed increment gives repeatable phase-specific random streams.
const UInt64 SEED_STRIDE = 0x9e3779b97f4a7c15ULL;

// --- Settings: search effort ---
// Fleet-size thresholds count taxis, not requests or nodes.
constexpr int SMALL_FLEET_TAXI_LIMIT = 48; // Enable repeated selection and pricing above this size.
constexpr int GROUP_MASTER_TAXI_LIMIT = 64;
constexpr int LONG_GENERATION_TAXI_LIMIT = 64;
constexpr int LARGE_FLEET_TAXI_LIMIT = 96;

// Phase budgets in seconds, with the output margin included in the search deadline.
constexpr int SEARCH_SECONDS = 299;
constexpr int INITIAL_LARGE_DFS_SECONDS = 2;
constexpr int INITIAL_SMALL_DFS_SECONDS = 40;
constexpr int INITIAL_SELECTION_SECONDS = 20;
constexpr int LARGE_GENERATION_RESERVE_SECONDS = 8;
constexpr int SMALL_GENERATION_RESERVE_SECONDS = 4;
constexpr int SMALL_GENERATION_SECONDS = 100;
constexpr int SELECTION_ROUND_SECONDS = 6;
constexpr int REBUILD_ROUND_SECONDS = 6;
constexpr int PRICING_GUARD_SECONDS = 2;
constexpr int PRICED_GENERATION_SECONDS = 3;
constexpr int FINAL_SELECTION_RESERVE_SECONDS = 2;
constexpr int FINAL_SELECTION_SECONDS = 40;
constexpr int FINAL_POLISH_CYCLES = 2;

// Route generation and randomized rebuilding.
constexpr int DEFAULT_BANK_ROUTE_LIMIT = 1000;
constexpr int LARGE_FLEET_BANK_LIMIT = 400;
constexpr int DFS_PREFIX_LIMIT = 1000000;
constexpr int DFS_CLOCK_INTERVAL = 64;
constexpr int LANDMARK_LIMIT = 8;
constexpr int LANDMARK_CLOCK_INTERVAL = 64;
constexpr Real LANDMARK_MAX_SECONDS = .6;
constexpr Real LANDMARK_BUDGET_FRACTION = .25;
constexpr int REBUILD_RESTART_INTERVAL = 8;
constexpr Real REBUILD_PROFIT_RETENTION = .995;
constexpr int REBUILD_POLISH_INTERVAL = 2;
constexpr int MIN_REMOVED_REQUESTS = 4;
constexpr int REQUEST_REMOVAL_DIVISOR = 12;
constexpr int TAXI_REMOVAL_DIVISOR = 12;
constexpr int PRICED_BANK_STARTS = 3;
constexpr int PRICED_MOVE_LIMIT = 48;
constexpr int PRICED_RESTART_LIMIT = 8;

// LP, group selection, and request pricing.
constexpr int SIMPLEX_ITERATION_LIMIT = 10000;
constexpr int SIMPLEX_CLOCK_MASK = 7; // Check time every eight iterations, including iteration zero.
constexpr int GROUP_MIN_TAXIS = 8;
constexpr int GROUP_TAXI_INCREMENT = 4;
constexpr int GROUP_WIDTH_LEVELS = 4;
constexpr int GROUP_RANDOM_SEED_CANDIDATES = 24;
constexpr int GROUP_OVERLAP_CANDIDATES = 32;
constexpr int GROUP_ROUTE_CANDIDATES = 64;
constexpr int GROUP_SOLVE_MILLISECONDS = 150;
constexpr int GROUP_STAGNATION_ROUNDS_PER_TAXI = 12;
constexpr int RANKED_LP_SECONDS = 6;
constexpr int RANKED_FIRST_SOLVE_SECONDS = 4;
constexpr int RANKED_WIDE_SOLVE_SECONDS = 6;
constexpr int RANKED_GROUP_GUARD_MILLISECONDS = 100;
constexpr int PRICE_ITERATION_LIMIT = 192;
constexpr Real PRICE_STEP_FACTOR = .7;

// --- Data structures ---
struct Request {
	int pickup_node, delivery_node;
	bool is_passenger;
	Real weight, pickup_earliest, pickup_latest, delivery_earliest, delivery_latest, service_time, revenue;
};

// Each taxi keeps its best route for each sorted set of served requests.
struct StoredRoute {
	IntVector route;
	Real profit;
};

struct Instance {
	int taxi_count, passenger_count, parcel_count, node_count, request_count;
	IntVector taxi_origin;
	RealVector taxi_capacity, travel_time, travel_cost;
	vector<Request> requests;
	IntVector node_request, is_delivery;
};

// A master column is one complete taxi route and the requests it covers.
struct MasterColumn {
	size_t taxi;
	IndexVector requests;
	Real profit;
};

struct MasterResult {
	IntVector selected_columns;
	Real profit = 0;
	bool timed_out = false;
};

struct PackingMatrix {
	const vector<MasterColumn> *columns;
	vector<IndexVector> column_rows;
	IndexVector row_resources;
	size_t dimension = 0, taxi_count;
};

// Revised simplex state: basis inverse, basic values, and resource dual prices.
struct PackingLP {
	const PackingMatrix *matrix;
	size_t row_count;
	RealVector basis_inverse, basic_values, dual_prices, right_hand_sides, bound_signs;
	IndexVector basis_columns;
	vector<unsigned char> in_basis;
	IntVector column_bound_rows;
	bool timed_out = false;
};

// Positive, globally unique route versions invalidate move caches after every update.
struct SearchState {
	Fleet routes;
	IntVector request_owner;
	RealVector route_profit;
	vector<UInt64> route_version;
	Real profit = 0;
};

// Both positions refer to the original route; insert delivery before pickup.
struct InsertionChoice {
	int request = -1, taxi = -1, pickup_position = 0, delivery_position = 0;
	Real profit_delta = -INF;
};

// A segment maps first-node arrival a to max(a + duration, earliest_departure).
// Arrival must be <= latest_arrival; load extrema include the empty prefix.
struct Segment {
	int first_node = 0, last_node = 0;
	Real duration = 0, earliest_departure = -INF, latest_arrival = INF, load_change = 0, minimum_load = 0,
		 maximum_load = 0;
	bool feasible = true;
};

// Disjoint sparse table: join two summaries to query any contiguous route range.
struct RouteSegmentCache {
	Route nodes;
	vector<Segment> node_segments;
	vector<vector<Segment>> range_table;
};

// Cache request/taxi insertions until that taxi's route version changes.
struct InsertionCache {
	vector<vector<UInt64>> route_version;
	vector<vector<InsertionChoice>> choices;
};

struct ReplacementChoice {
	int outgoing_request = -1;
	InsertionChoice insertion;
	Real profit_delta = -INF;
};

struct ReplacementCache {
	vector<UInt64> route_version;
	vector<char> is_unserved;
	vector<vector<ReplacementChoice>> choices;
};

using RouteBank = map<IntVector, StoredRoute>;

// --- Shared working data ---
// One instance per process. Preserve cache lifetimes and globally unique route versions.
// DFS request states: 0 unseen, 1 picked up, 2 delivered.
Instance instance;
UInt64 next_route_version = 1, random_seed = 1;
mt19937_64 random_engine;
vector<RouteBank> route_bank;
RealVector bank_profit_floor;
int bank_route_limit = DEFAULT_BANK_ROUTE_LIMIT;
Deadline search_deadline;
RealVector delivery_time_lower_bound;
vector<IntVector> successor_requests;
IntVector dfs_request_state, dfs_route, dfs_requests;
size_t dfs_visited_prefixes, dfs_prefix_limit = DFS_PREFIX_LIMIT;
Real priced_score_floor = -INF;
Deadline dfs_deadline;
bool dfs_stopped;
RealVector dfs_request_prices;
map<IntVector, pair<Real, Route>> priced_route_bank;
size_t dfs_generation_calls = 0;
vector<RouteSegmentCache> route_segment_cache;
ReplacementCache replacement_cache;
size_t fleet_generation_calls = 0;
size_t priced_replacement_calls = 0;

// --- Search declarations ---
bool enumerate_routes(Deadline deadline, const RealVector &prices = {});
void enumerate_route_prefix(
	int taxi, int last_node, Real departure_time, Real total_cost, Real revenue, Real load, int open_parcels);
void rebuild_fleets(SearchState &incumbent, Deadline deadline);
void destroy_solution(SearchState &state, int round_index);
void generate_priced_replacements(SearchState &state, Deadline deadline);
void select_fleet_routes(SearchState &state, Deadline deadline);
MasterResult solve_route_master(const vector<MasterColumn> &columns,
	size_t taxi_count,
	size_t request_count,
	const IntVector &initial_columns,
	Deadline deadline);
MasterResult solve_taxi_groups(const vector<MasterColumn> &columns,
	size_t taxi_count,
	size_t request_count,
	const IntVector &initial_columns,
	Deadline deadline,
	UInt64 group_seed,
	const RealVector &request_prices);
MasterResult solve_ranked_master(const vector<MasterColumn> &columns,
	int taxi_count,
	int request_count,
	const IntVector &initial_columns,
	Deadline deadline);
RealVector estimate_request_prices(Real incumbent_profit = 0);
void improve_solution(SearchState &state, Deadline deadline, bool full_neighborhoods, int cycles = 3);
InsertionChoice find_route_insertion(
	int request, int taxi, const Route &route, Real minimum_profit_delta = FEASIBILITY_EPSILON);
InsertionChoice find_fleet_insertion(SearchState &state, int request, InsertionCache &cache);
bool improve_by_removal(SearchState &state);
bool improve_by_insertion(SearchState &state, InsertionCache &cache);
bool improve_by_replacement(SearchState &state, Deadline deadline);
bool improve_by_tail_exchange(SearchState &state, Deadline deadline);
bool improve_by_relocation(SearchState &state, Deadline deadline);
bool improve_by_swap(SearchState &state, Deadline deadline);

// --- Helper declarations ---
Instance read_instance();
optional<Real> evaluate_route(const Instance &input, int taxi, const IntVector &route);
Deadline deadline_after(Real seconds);
Real travel_time(int from_node, int to_node);
Real travel_cost(int from_node, int to_node);
IntVector served_requests(const Route &route);
Real route_cost(int taxi, const Route &route);
Real route_profit(int taxi, const Route &route);
Real fleet_profit(const Fleet &fleet);
void store_route(int taxi, const Route &route, bool enforce_limit = false, Real profit = -INF);
void store_fleet(const Fleet &fleet);
SearchState make_search_state(const Fleet &fleet);
void update_route(SearchState &state, int taxi, const Route &route);
void update_route_pair(
	SearchState &state, int first_taxi, const Route &first_route, int second_taxi, const Route &second_route);
void apply_insertion(SearchState &state, InsertionChoice choice);
bool is_insertion_boundary(const Route &route, int position);
Route remove_request(const Route &route, int request);
Route insert_request(Route route, int request, int pickup_position, int delivery_position);
IntVector empty_load_cuts(const Route &route);
void build_delivery_lower_bounds(Deadline deadline);
Segment make_node_segment(int node);
Segment join_segments(Segment left, const Segment &right);
bool is_segment_feasible(int taxi, const Segment &segment);
RouteSegmentCache &cached_route_segments(int taxi, const Route &route);
Segment segment_range(const RouteSegmentCache &segments, int begin, int end);
PackingMatrix build_packing_matrix(
	const vector<MasterColumn> &columns, size_t taxi_count, size_t request_count);
PackingLP initialize_lp(const PackingMatrix &matrix);
Real column_coefficient(const PackingLP &lp, size_t row, size_t column);
Real reduced_cost(const PackingLP &lp, size_t column);
void pivot_basis(PackingLP &lp, size_t leaving, size_t entering);
void primal_simplex(PackingLP &lp, Deadline deadline);
void add_column_bound(PackingLP &lp, size_t column, bool include);
void dual_simplex(PackingLP &lp, Deadline deadline);
RealVector primal_values(const PackingLP &lp);
Real dual_upper_bound(const PackingLP &lp);
RealVector request_prices_from_lp(const PackingLP &lp, size_t request_count);

// --- Search pipeline ---
int main(int argc, char **argv) {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	// Include input time in the search budget; prepare an empty incumbent fleet.
	auto start_time = Clock::now();
	if (argc > 1) {
		random_seed = strtoull(argv[1], nullptr, 10);
	}
	random_engine.seed(random_seed);
	instance = read_instance();
	search_deadline = start_time + Seconds(SEARCH_SECONDS);
	if (instance.taxi_count > LARGE_FLEET_TAXI_LIMIT) {
		bank_route_limit = LARGE_FLEET_BANK_LIMIT;
	}
	route_bank.resize(instance.taxi_count);
	bank_profit_floor.assign(instance.taxi_count, -INF);
	SearchState state = make_search_state(Fleet(instance.taxi_count));

	// Initial enumeration; large fleets use a short seed bank and an early master solve.
	bool complete = enumerate_routes(min(search_deadline,
		deadline_after(instance.taxi_count > LARGE_FLEET_TAXI_LIMIT ? INITIAL_LARGE_DFS_SECONDS
																	: INITIAL_SMALL_DFS_SECONDS)));
	if (!complete || instance.taxi_count > SMALL_FLEET_TAXI_LIMIT) {
		int phase = 0;
		if (instance.taxi_count > SMALL_FLEET_TAXI_LIMIT) {
			select_fleet_routes(state, min(search_deadline, deadline_after(INITIAL_SELECTION_SECONDS)));
		}

		// Alternate fleet rebuilding, selection, and priced route generation.
		// Large fleets stop generation 8 seconds before the 299-second search deadline.
		auto generation_deadline = instance.taxi_count > LONG_GENERATION_TAXI_LIMIT
			? search_deadline - Seconds(LARGE_GENERATION_RESERVE_SECONDS)
			: min(search_deadline - Seconds(SMALL_GENERATION_RESERVE_SECONDS),
				  deadline_after(SMALL_GENERATION_SECONDS));
		do {
			rebuild_fleets(state,
				instance.taxi_count > SMALL_FLEET_TAXI_LIMIT
					? min(generation_deadline, deadline_after(REBUILD_ROUND_SECONDS))
					: generation_deadline);
			if (instance.taxi_count > SMALL_FLEET_TAXI_LIMIT &&
				Clock::now() + Seconds(PRICING_GUARD_SECONDS) < generation_deadline) {
				select_fleet_routes(state, min(generation_deadline, deadline_after(SELECTION_ROUND_SECONDS)));
				if (Clock::now() < generation_deadline) {
					if (++phase % 2) {
						generate_priced_replacements(
							state, min(generation_deadline, deadline_after(PRICED_GENERATION_SECONDS)));
					} else {
						enumerate_routes(min(generation_deadline, deadline_after(PRICED_GENERATION_SECONDS)),
							estimate_request_prices(state.profit));
					}
				}
			}
		} while (instance.taxi_count > SMALL_FLEET_TAXI_LIMIT && Clock::now() < generation_deadline);
	}

	// Reserve the final seconds for selection and local descent; allow output before 300 s.
	select_fleet_routes(state,
		min(search_deadline - Seconds(FINAL_SELECTION_RESERVE_SECONDS),
			deadline_after(FINAL_SELECTION_SECONDS)));
	if (Clock::now() < search_deadline) {
		improve_solution(state, search_deadline, true, FINAL_POLISH_CYCLES);
	}

	// Output only the objective, followed by one node sequence per taxi.
	Real value = 0;
	for (int taxi = 0; taxi < instance.taxi_count; taxi++) {
		value -= route_cost(taxi, state.routes[taxi]);
	}
	for (int request = 0; request < instance.request_count; request++) {
		if (state.request_owner[request] >= 0) {
			value += instance.requests[request].revenue;
		}
	}
	cout << setprecision(17) << value << '\n';
	for (auto &route : state.routes) {
		cout << route.size();
		for (int node : route) {
			cout << ' ' << node;
		}
		cout << '\n';
	}
	return 0;
}

// --- Route generation: bounded DFS ---
bool enumerate_routes(Deadline deadline, const RealVector &prices) {
	// Vary request-price strength between calls while keeping each seed reproducible.
	dfs_request_prices = prices;
	auto call = dfs_generation_calls++;
	if (!prices.empty()) {
		random_engine.seed(random_seed + SEED_STRIDE * call);
		Real scale[] = {1, .8, 1.2};
		for (auto &price : dfs_request_prices) {
			price *= scale[call % 3];
		}
	}
	// Build reusable pickup candidates, ordered by latest possible departure.
	if (successor_requests.empty()) {
		build_delivery_lower_bounds(deadline);
		successor_requests.resize(instance.node_count + 1);
		for (int node = 1; node <= instance.node_count; node++) {
			auto &candidates = successor_requests[node];
			for (int request = 0; request < instance.request_count; request++) {
				if (travel_time(node, instance.requests[request].pickup_node) <=
					instance.requests[request].pickup_latest + FEASIBILITY_EPSILON) {
					candidates.push_back(request);
				}
			}
			stable_sort(candidates.begin(), candidates.end(), [&](int left, int right) {
				return instance.requests[left].pickup_latest -
					travel_time(node, instance.requests[left].pickup_node) >
					instance.requests[right].pickup_latest -
					travel_time(node, instance.requests[right].pickup_node);
			});
		}
	}
	bool complete = true;
	dfs_request_state.assign(instance.request_count, 0);
	for (int taxi = 0; taxi < instance.taxi_count; taxi++) {
		// Divide remaining time among remaining taxis and reset this taxi's DFS state.
		auto current_time = Clock::now();
		if (current_time >= deadline) {
			return false;
		}
		auto taxi_deadline = current_time + (deadline - current_time) / (instance.taxi_count - taxi);
		dfs_deadline = taxi_deadline;
		dfs_stopped = false;
		dfs_visited_prefixes = 0;
		dfs_prefix_limit = DFS_PREFIX_LIMIT;
		dfs_route.clear();
		dfs_requests.clear();
		priced_route_bank.clear();
		priced_score_floor = -INF;

		// Ordinary DFS starts at the depot; priced DFS diversifies the first pickup.
		if (prices.empty()) {
			enumerate_route_prefix(taxi, instance.taxi_origin[taxi], 0, 0, 0, 0, 0);
		} else {
			IntVector first_requests;
			for (int request : successor_requests[instance.taxi_origin[taxi]]) {
				if (instance.requests[request].weight <= instance.taxi_capacity[taxi] + FEASIBILITY_EPSILON &&
					travel_time(instance.taxi_origin[taxi], instance.requests[request].pickup_node) <=
						instance.requests[request].pickup_latest + FEASIBILITY_EPSILON) {
					first_requests.push_back(request);
				}
			}
			shuffle(first_requests.begin(), first_requests.end(), random_engine);
			// Share each taxi's time and prefix budget across its shuffled first pickups.
			int remaining_starts = first_requests.size();
			for (int request : first_requests) {
				current_time = Clock::now();
				if (current_time >= taxi_deadline || dfs_visited_prefixes >= DFS_PREFIX_LIMIT) {
					complete = false;
					break;
				}
				dfs_deadline = current_time + (taxi_deadline - current_time) / remaining_starts;
				dfs_prefix_limit = dfs_visited_prefixes +
					max<size_t>(1, (DFS_PREFIX_LIMIT - dfs_visited_prefixes) / remaining_starts);
				dfs_stopped = false;
				auto &request_data = instance.requests[request];
				Real arrival = travel_time(instance.taxi_origin[taxi], request_data.pickup_node),
					 departure_time = max(arrival, request_data.pickup_earliest) + request_data.service_time,
					 initial_cost = travel_cost(instance.taxi_origin[taxi], request_data.pickup_node);
				dfs_requests.push_back(request);
				dfs_route.push_back(request_data.pickup_node);
				dfs_request_state[request] = 1;
				if (request_data.is_passenger) {
					dfs_request_state[request] = 2;
					dfs_route.push_back(request_data.delivery_node);
					enumerate_route_prefix(taxi, request_data.delivery_node,
						departure_time + travel_time(request_data.pickup_node, request_data.delivery_node) +
							request_data.service_time,
						initial_cost + travel_cost(request_data.pickup_node, request_data.delivery_node),
						request_data.revenue, 0, 0);
					dfs_route.pop_back();
				} else {
					enumerate_route_prefix(taxi, request_data.pickup_node, departure_time, initial_cost,
						request_data.revenue, request_data.weight, 1);
				}
				dfs_request_state[request] = 0;
				dfs_requests.pop_back();
				dfs_route.pop_back();
				complete &= !dfs_stopped;
				remaining_starts--;
			}
		}
		complete &= !dfs_stopped;

		// Transfer priced candidates to the persistent bank using their actual profits.
		for (auto &[ids, priced_route] : priced_route_bank) {
			store_route(taxi, priced_route.second);
		}
	}
	return complete;
}

// Stop by deadline or prefix count; save profitable prefixes with no open parcels.
void enumerate_route_prefix(int taxi,
	int last_node,
	Real departure_time,
	Real total_cost,
	Real revenue,
	Real load,
	int open_parcels) {
	// Stop the entire DFS call when its prefix or time budget is exhausted.
	if (dfs_stopped) {
		return;
	}
	if (++dfs_visited_prefixes >= dfs_prefix_limit ||
		(dfs_visited_prefixes % DFS_CLOCK_INTERVAL == 0 && Clock::now() >= dfs_deadline)) {
		dfs_stopped = true;
		return;
	}

	// Prune when even a delivery lower bound misses an onboard parcel's deadline.
	for (int request : dfs_requests) {
		if (dfs_request_state[request] == 1 &&
			departure_time +
					delivery_time_lower_bound[(last_node - 1) * instance.parcel_count + request -
						instance.passenger_count] >
				instance.requests[request].delivery_latest + FEASIBILITY_EPSILON) {
			return;
		}
	}
	// A closed profitable prefix is a complete candidate route; extensions may still improve it.
	if (!open_parcels && revenue - total_cost > FEASIBILITY_EPSILON) {
		if (dfs_request_prices.empty()) {
			store_route(taxi, dfs_route, true, revenue - total_cost);
		} else {
			Real score = revenue - total_cost;
			for (int request : dfs_requests) {
				score -= dfs_request_prices[request];
			}
			if (score > FEASIBILITY_EPSILON &&
				(priced_route_bank.size() < (size_t)bank_route_limit || score >= priced_score_floor)) {
				auto requests = served_requests(dfs_route);
				auto entry = priced_route_bank.find(requests);
				if (entry == priced_route_bank.end() || entry->second.first < score) {
					priced_route_bank[requests] = {score, dfs_route};
					if (priced_route_bank.size() > (size_t)bank_route_limit) {
						priced_route_bank.erase(min_element(
							priced_route_bank.begin(), priced_route_bank.end(), [](auto &left, auto &right) {
								return left.second.first < right.second.first;
							}));
						priced_score_floor = min_element(
							priced_route_bank.begin(), priced_route_bank.end(), [](auto &left, auto &right) {
								return left.second.first < right.second.first;
							})->second.first;
					}
				}
			}
		}
	}
	// Enumerate onboard deliveries and new pickups; priced DFS rotates their event order.
	size_t pickup_count = successor_requests[last_node].size(), delivery_count = dfs_requests.size();
	UInt64 hash = 0;
	if (!dfs_request_prices.empty()) {
		auto &candidates = successor_requests[last_node];
		pickup_count = partition_point(candidates.begin(), candidates.end(), [&](int request) {
			return departure_time <= instance.requests[request].pickup_latest -
				travel_time(last_node, instance.requests[request].pickup_node) + FEASIBILITY_EPSILON;
		}) - candidates.begin();
		// Mix the phase, taxi, and last node to rotate priced DFS event order.
		hash = random_seed + SEED_STRIDE * (dfs_generation_calls - 1) + taxi * 0x94d049bb133111ebULL +
			last_node * 0xbf58476d1ce4e5b9ULL;
		hash = (hash ^ (hash >> 30)) * 0xbf58476d1ce4e5b9ULL;
		hash ^= hash >> 27;
	}
	size_t events = delivery_count + pickup_count, offset = events && (hash & 1) ? hash % events : 0;
	for (size_t i = 0; i < events; i++) {
		auto event = offset ? (i + offset) % events : i;
		if (event < delivery_count) {
			// Deliver one onboard parcel, recurse, then restore the prefix.
			int request = dfs_requests[event];
			if (dfs_request_state[request] != 1 || instance.requests[request].is_passenger) {
				continue;
			}
			auto &request_data = instance.requests[request];
			Real arrival = departure_time + travel_time(last_node, request_data.delivery_node);
			if (arrival > request_data.delivery_latest + FEASIBILITY_EPSILON) {
				continue;
			}
			dfs_request_state[request] = 2;
			dfs_route.push_back(request_data.delivery_node);
			enumerate_route_prefix(taxi, request_data.delivery_node,
				max(arrival, request_data.delivery_earliest) + request_data.service_time,
				total_cost + travel_cost(last_node, request_data.delivery_node), revenue,
				load - request_data.weight, open_parcels - 1);
			dfs_route.pop_back();
			dfs_request_state[request] = 1;
			if (dfs_stopped) {
				return;
			}
			continue;
		}

		// Reject pickups that violate time, capacity, or the request's current state.
		int request = successor_requests[last_node][event - delivery_count];
		auto &request_data = instance.requests[request];
		Real arc_time = travel_time(last_node, request_data.pickup_node);
		Real rounding_margin = FEASIBILITY_EPSILON +
			8 * numeric_limits<Real>::epsilon() * max({1.0, abs(request_data.pickup_latest), abs(arc_time)});
		if (departure_time > request_data.pickup_latest - arc_time + rounding_margin) {
			if (dfs_request_prices.empty()) {
				break;
			}
			continue;
		}
		Real arrival = departure_time + arc_time;
		if (arrival > request_data.pickup_latest + FEASIBILITY_EPSILON) {
			continue;
		}
		if (dfs_request_state[request] ||
			load + request_data.weight > instance.taxi_capacity[taxi] + FEASIBILITY_EPSILON) {
			continue;
		}
		Real next_departure = max(arrival, request_data.pickup_earliest) + request_data.service_time,
			 next_cost = total_cost + travel_cost(last_node, request_data.pickup_node);
		dfs_request_state[request] = 1;
		dfs_requests.push_back(request);
		dfs_route.push_back(request_data.pickup_node);

		// Serve passengers as adjacent pickup/delivery blocks; leave parcels onboard.
		if (request_data.is_passenger) {
			dfs_request_state[request] = 2;
			dfs_route.push_back(request_data.delivery_node);
			enumerate_route_prefix(taxi, request_data.delivery_node,
				next_departure + travel_time(request_data.pickup_node, request_data.delivery_node) +
					request_data.service_time,
				next_cost + travel_cost(request_data.pickup_node, request_data.delivery_node),
				revenue + request_data.revenue, load, open_parcels);
			dfs_route.pop_back();
			dfs_request_state[request] = 1;
		} else {
			enumerate_route_prefix(taxi, request_data.pickup_node, next_departure, next_cost,
				revenue + request_data.revenue, load + request_data.weight, open_parcels + 1);
		}

		// Backtrack so the next event sees exactly the same incoming prefix.
		dfs_route.pop_back();
		dfs_requests.pop_back();
		dfs_request_state[request] = 0;
		if (dfs_stopped) {
			return;
		}
	}
}

// --- Route generation: fleet rebuilding ---
// Randomized destroy/rebuild rounds add diverse routes to the bank.
// Periodically restart from the best fleet, including after a sufficient profit loss.
void rebuild_fleets(SearchState &incumbent, Deadline deadline) {
	random_engine.seed(random_seed + SEED_STRIDE * fleet_generation_calls++);
	SearchState current = incumbent, best_state = incumbent;
	InsertionCache cache;
	IntVector order(instance.request_count);
	iota(order.begin(), order.end(), 0);
	int round_index = 0;
	while (Clock::now() < deadline) {
		// Perturb the current fleet; periodically return to the best fleet found in this call.
		if (round_index) {
			if (round_index % REBUILD_RESTART_INTERVAL == 0 ||
				current.profit < best_state.profit * REBUILD_PROFIT_RETENTION) {
				current = best_state;
			}
			destroy_solution(current, round_index);
		}

		// Reinsert unserved requests in random order using cached best taxi positions.
		shuffle(order.begin(), order.end(), random_engine);
		for (int request : order) {
			if (Clock::now() >= deadline) {
				break;
			}
			if (current.request_owner[request] >= 0) {
				continue;
			}
			auto choice = find_fleet_insertion(current, request, cache);
			if (choice.taxi >= 0) {
				apply_insertion(current, choice);
			}
		}
		// Complete profitable insertions, then occasionally run the cheaper descent neighborhoods.
		while (Clock::now() < deadline && improve_by_insertion(current, cache)) {
		}
		if (round_index % REBUILD_POLISH_INTERVAL == 0 && Clock::now() < deadline) {
			improve_solution(current, deadline, false);
		}

		// Keep the best complete fleet even when later rounds deliberately lose profit.
		if (current.profit > best_state.profit) {
			best_state = current;
		}
		round_index++;
	}
	// Return the best fleet; preserve its routes as candidates for later master solves.
	incumbent = best_state;
	store_fleet(incumbent.routes);
}

// Cycle through whole-taxi, random, related, and profit-based removals.
void destroy_solution(SearchState &state, int round_index) {
	int count = max(MIN_REMOVED_REQUESTS,
		(int)count_if(state.request_owner.begin(), state.request_owner.end(), [](int taxi) {
			return taxi >= 0;
		}) / REQUEST_REMOVAL_DIVISOR);
	// Mode 0: empty a few taxis to free whole route combinations.
	if (round_index % 4 == 0) {
		for (int i = 0,
				 taxi_removals = 2 + random_engine() % max(2, instance.taxi_count / TAXI_REMOVAL_DIVISOR);
			i < taxi_removals; i++) {
			int taxi = random_engine() % instance.taxi_count;
			if (!state.routes[taxi].empty()) {
				update_route(state, taxi, {});
			}
		}
		return;
	}
	IntVector served_ids;
	for (int request = 0; request < instance.request_count; request++) {
		if (state.request_owner[request] >= 0) {
			served_ids.push_back(request);
		}
	}
	if (served_ids.empty()) {
		return;
	}
	// Mode 1: remove random served requests, retaining feasibility after each removal.
	if (round_index % 4 == 1) {
		served_ids.resize(instance.request_count);
		iota(served_ids.begin(), served_ids.end(), 0);
		shuffle(served_ids.begin(), served_ids.end(), random_engine);
		int removed = 0;
		for (int request : served_ids) {
			if (state.request_owner[request] >= 0) {
				int taxi = state.request_owner[request];
				auto route = remove_request(state.routes[taxi], request);
				if (evaluate_route(instance, taxi, route)) {
					update_route(state, taxi, route);
					if (++removed == count) {
						break;
					}
				}
			}
		}
		return;
	}
	// Mode 2 removes requests related to an anchor; mode 3 favors the best removal gain.
	int anchor = round_index % 4 == 2 ? served_ids[random_engine() % served_ids.size()] : 0;
	for (int removed = 0; removed < count; removed++) {
		int chosen = -1;
		Real best = -INF;
		Route reduced_route;
		for (int request = 0; request < instance.request_count; request++) {
			if (state.request_owner[request] >= 0) {
				int taxi = state.request_owner[request];
				auto route = remove_request(state.routes[taxi], request);
				auto evaluated_profit = evaluate_route(instance, taxi, route);
				if (!evaluated_profit) {
					continue;
				}
				Real score = *evaluated_profit - state.route_profit[taxi];
				if (round_index % 4 == 2) {
					score = -travel_time(instance.requests[anchor].pickup_node,
								instance.requests[request].pickup_node) -
						travel_time(
							instance.requests[request].pickup_node, instance.requests[anchor].pickup_node) -
						abs(instance.requests[anchor].pickup_earliest -
							instance.requests[request].pickup_earliest);
				}
				if (score > best) {
					best = score;
					chosen = request;
					reduced_route = move(route);
				}
			}
		}
		if (chosen < 0) {
			break;
		}
		update_route(state, state.request_owner[chosen], reduced_route);
	}
}

// --- Route generation: priced replacements ---
// Generate routes by insertion/removal/replacement under request opportunity costs.
// Start from the incumbent route and several bank routes, with bounded restarts.
void generate_priced_replacements(SearchState &state, Deadline deadline) {
	// Penalize expensive-to-share requests and vary that penalty across generation calls.
	auto prices = estimate_request_prices(state.profit);
	auto call = priced_replacement_calls++;
	random_engine.seed(random_seed + SEED_STRIDE * call);
	Real scale[] = {1, .8, 1.2};
	for (auto &price : prices) {
		price *= scale[call % 3];
	}
	for (int taxi = 0; taxi < instance.taxi_count && Clock::now() < deadline; taxi++) {
		// Share time fairly; seed this taxi's search from its incumbent and top bank routes.
		auto current_time = Clock::now(),
			 taxi_deadline = current_time + (deadline - current_time) / (instance.taxi_count - taxi);
		vector<pair<Real, const StoredRoute *>> ranked;
		for (auto &[ids, stored_route] : route_bank[taxi]) {
			Real reduced_profit = stored_route.profit;
			for (int request : ids) {
				reduced_profit -= prices[request];
			}
			ranked.push_back({reduced_profit, &stored_route});
		}
		int count = min<int>(PRICED_BANK_STARTS, ranked.size());
		partial_sort(ranked.begin(), ranked.begin() + count, ranked.end(), [](auto &left, auto &right) {
			return left.first > right.first;
		});
		vector<Route> seeds{state.routes[taxi]};
		for (int i = 0; i < count; i++) {
			seeds.push_back(ranked[i].second->route);
		}
		SearchState candidate_state = make_search_state(Fleet(instance.taxi_count));
		update_route(candidate_state, taxi, seeds[0]);
		int restarts = 0;
		for (int iteration = 0; iteration < PRICED_MOVE_LIMIT && Clock::now() < taxi_deadline; iteration++) {
			Real best = FEASIBILITY_EPSILON;
			Route chosen;
			bool changed = false;
			auto score = [&](const Route &route) {
				Real reduced_profit = route_profit(taxi, route);
				for (int request : served_requests(route)) {
					reduced_profit -= prices[request];
				}
				return reduced_profit;
			};
			Real previous_score = score(candidate_state.routes[taxi]);

			// Try adding an unserved request, accounting for its request-price penalty.
			for (int request = 0; request < instance.request_count && Clock::now() < taxi_deadline;
				request++) {
				if (candidate_state.request_owner[request] < 0) {
					auto choice = find_route_insertion(
						request, taxi, candidate_state.routes[taxi], best + prices[request]);
					if (choice.taxi >= 0 && choice.profit_delta - prices[request] > best) {
						best = choice.profit_delta - prices[request];
						chosen = insert_request(candidate_state.routes[taxi], request, choice.pickup_position,
							choice.delivery_position);
						changed = true;
					}
				}
			}
			// Compare removals and one-for-one replacements by their change in priced profit.
			for (int outgoing_request : served_requests(candidate_state.routes[taxi])) {
				auto route = remove_request(candidate_state.routes[taxi], outgoing_request);
				Real removal_gain = route_profit(taxi, route) - candidate_state.route_profit[taxi];
				if (evaluate_route(instance, taxi, route) && score(route) - previous_score > best) {
					best = score(route) - previous_score;
					chosen = route;
					changed = true;
				}
				for (int incoming_request = 0;
					incoming_request < instance.request_count && Clock::now() < taxi_deadline;
					incoming_request++) {
					if (candidate_state.request_owner[incoming_request] < 0) {
						auto choice = find_route_insertion(incoming_request, taxi, route,
							best - removal_gain - prices[outgoing_request] + prices[incoming_request]);
						if (choice.taxi >= 0 &&
							removal_gain + choice.profit_delta + prices[outgoing_request] -
									prices[incoming_request] >
								best) {
							best = removal_gain + choice.profit_delta + prices[outgoing_request] -
								prices[incoming_request];
							chosen = insert_request(
								route, incoming_request, choice.pickup_position, choice.delivery_position);
							changed = true;
						}
					}
				}
			}
			// At a local optimum, restart from another seed and remove a few requests for diversity.
			if (!changed) {
				if (++restarts > PRICED_RESTART_LIMIT) {
					break;
				}
				update_route(candidate_state, taxi, seeds[restarts % seeds.size()]);
				auto ids = served_requests(candidate_state.routes[taxi]);
				if (!ids.empty()) {
					shuffle(ids.begin(), ids.end(), random_engine);
					ids.resize(min<size_t>(ids.size(), 1 + random_engine() % 3));
					Route reduced;
					for (int node : candidate_state.routes[taxi]) {
						if (find(ids.begin(), ids.end(), instance.node_request[node]) == ids.end()) {
							reduced.push_back(node);
						}
					}
					if (evaluate_route(instance, taxi, reduced)) {
						update_route(candidate_state, taxi, reduced);
					}
				}
				continue;
			}
			// Apply the best priced move and store the resulting route with its actual profit.
			update_route(candidate_state, taxi, chosen);
			store_route(taxi, chosen);
		}
	}
}

// --- Route selection: master dispatch ---
// Build the master from the bank and retain the incumbent unless profit improves.
void select_fleet_routes(SearchState &state, Deadline deadline) {
	// Preserve incumbent routes and map every stored route to a master column.
	store_fleet(state.routes);
	vector<MasterColumn> columns;
	vector<Route> routes;
	IntVector initial(instance.taxi_count, -1);
	for (int taxi = 0; taxi < instance.taxi_count; taxi++) {
		for (auto &[ids, stored_route] : route_bank[taxi]) {
			if (ids == served_requests(state.routes[taxi])) {
				initial[taxi] = columns.size();
			}
			columns.push_back({(size_t)taxi, IndexVector(ids.begin(), ids.end()), stored_route.profit});
			routes.push_back(stored_route.route);
		}
	}
	// Choose a master strategy by fleet size; larger fleets use request opportunity costs.
	auto prices =
		instance.taxi_count > SMALL_FLEET_TAXI_LIMIT ? estimate_request_prices(state.profit) : RealVector{};
	MasterResult solved;
	if (instance.taxi_count > GROUP_MASTER_TAXI_LIMIT) {
		solved = solve_taxi_groups(
			columns, instance.taxi_count, instance.request_count, initial, deadline, random_seed, prices);
	} else if (instance.taxi_count > SMALL_FLEET_TAXI_LIMIT) {
		solved = solve_ranked_master(columns, instance.taxi_count, instance.request_count, initial, deadline);
	} else {
		solved = solve_route_master(columns, instance.taxi_count, instance.request_count, initial, deadline);
	}
	// Reconstruct the selected fleet and accept it only when actual profit improves.
	Fleet selected_fleet(instance.taxi_count);
	for (int taxi = 0; taxi < instance.taxi_count; taxi++) {
		if (solved.selected_columns[taxi] >= 0) {
			selected_fleet[taxi] = routes[solved.selected_columns[taxi]];
		}
	}
	if (fleet_profit(selected_fleet) > state.profit + FEASIBILITY_EPSILON) {
		state = make_search_state(selected_fleet);
	}
}

// --- Route selection: branch and bound ---
// Branch on a fractional route; greedily round the LP to update the incumbent.
MasterResult solve_route_master(const vector<MasterColumn> &columns,
	size_t taxi_count,
	size_t request_count,
	const IntVector &initial_columns,
	Deadline deadline) {
	// Start with the supplied feasible fleet so a timeout still returns an incumbent.
	MasterResult result;
	result.selected_columns = initial_columns;
	for (auto column_id : initial_columns) {
		if (column_id >= 0) {
			result.profit += columns[column_id].profit;
		}
	}
	if (columns.empty()) {
		return result;
	}
	// Solve the root LP relaxation before exploring include/exclude branches.
	auto matrix = build_packing_matrix(columns, taxi_count, request_count);
	auto root = initialize_lp(matrix);
	primal_simplex(root, deadline);
	if (root.timed_out) {
		result.timed_out = true;
		return result;
	}
	// Integer route profits permit rounding the upper bound down before pruning.
	bool integral = true;
	for (auto &column : columns) {
		integral &= column.profit == round(column.profit);
	}
	vector<unsigned char> fixed(taxi_count, 0), occupied(request_count, 0), forbidden(columns.size(), 0);
	IntVector selected_columns(taxi_count, -1);
	// Track conflicts with decisions fixed along the current branch-and-bound path.
	auto compatible = [&](size_t column_id) {
		auto &column = columns[column_id];
		if (fixed[column.taxi] || forbidden[column_id]) {
			return false;
		}
		for (auto request : column.requests) {
			if (occupied[request]) {
				return false;
			}
		}
		return true;
	};
	function<void(const PackingLP &, Real)> visit = [&](const PackingLP &lp, Real profit) {
		if (result.timed_out) {
			return;
		}
		if (Clock::now() >= deadline) {
			result.timed_out = true;
			return;
		}
		// Prune this subtree if its LP upper bound cannot beat the incumbent.
		Real upper_bound = ::dual_upper_bound(lp);
		if (integral) {
			upper_bound = floor(upper_bound);
		}
		if (upper_bound <= result.profit + IMPROVEMENT_EPSILON) {
			return;
		}
		// Rank compatible routes by LP value, breaking ties by route profit.
		auto lp_values = primal_values(lp);
		IndexVector ranking;
		for (size_t j = 0; j < columns.size(); ++j) {
			if (compatible(j)) {
				ranking.push_back(j);
			}
		}
		if (ranking.empty()) {
			if (profit > result.profit + IMPROVEMENT_EPSILON) {
				result.profit = profit;
				result.selected_columns = selected_columns;
			}
			return;
		}
		stable_sort(ranking.begin(), ranking.end(), [&](auto left, auto right) {
			return lp_values[left] != lp_values[right] ? lp_values[left] > lp_values[right]
													   : columns[left].profit > columns[right].profit;
		});
		// Greedily round the ranked columns to obtain a feasible complete fleet.
		auto used_requests = occupied, assigned_taxis = fixed;
		auto candidate_columns = selected_columns;
		Real candidate = profit;
		for (auto column_id : ranking) {
			auto &column = columns[column_id];
			if (assigned_taxis[column.taxi]) {
				continue;
			}
			bool available = true;
			for (auto request : column.requests) {
				available &= !used_requests[request];
			}
			if (!available) {
				continue;
			}
			assigned_taxis[column.taxi] = 1;
			for (auto request : column.requests) {
				used_requests[request] = 1;
			}
			candidate_columns[column.taxi] = int(column_id);
			candidate += column.profit;
		}
		if (candidate > result.profit + IMPROVEMENT_EPSILON) {
			result.profit = candidate;
			result.selected_columns = move(candidate_columns);
		}
		if (upper_bound <= result.profit + IMPROVEMENT_EPSILON) {
			return;
		}
		// Prefer the most fractional route; fall back to the first compatible column.
		size_t branch_column = ranking.front();
		Real fractionality = -1;
		for (auto j : ranking) {
			if (lp_values[j] <= LP_FEASIBILITY_EPSILON || lp_values[j] >= 1 - LP_FEASIBILITY_EPSILON) {
				continue;
			}
			auto score = min(lp_values[j], 1 - lp_values[j]);
			if (score > fractionality) {
				fractionality = score;
				branch_column = j;
			}
		}
		auto &column = columns[branch_column];

		// Include this route, repair the LP by dual simplex, recurse, then undo conflicts.
		{
			auto child = lp;
			add_column_bound(child, branch_column, true);
			dual_simplex(child, deadline);
			if (child.timed_out) {
				result.timed_out = true;
				return;
			}
			fixed[column.taxi] = 1;
			for (auto request : column.requests) {
				occupied[request] = 1;
			}
			selected_columns[column.taxi] = int(branch_column);
			visit(child, profit + column.profit);
			selected_columns[column.taxi] = -1;
			fixed[column.taxi] = 0;
			for (auto request : column.requests) {
				occupied[request] = 0;
			}
		}
		// Explore the sibling branch with this route excluded, unless time has expired.
		if (!result.timed_out) {
			auto child = lp;
			add_column_bound(child, branch_column, false);
			dual_simplex(child, deadline);
			if (child.timed_out) {
				result.timed_out = true;
				return;
			}
			forbidden[branch_column] = 1;
			visit(child, profit);
			forbidden[branch_column] = 0;
		}
	};
	visit(root, 0);
	return result;
}

// --- Route selection: taxi groups ---
// Optimize overlapping taxi groups while keeping all outside routes fixed.
MasterResult solve_taxi_groups(const vector<MasterColumn> &columns,
	size_t taxi_count,
	size_t request_count,
	const IntVector &initial_columns,
	Deadline deadline,
	UInt64 group_seed,
	const RealVector &request_prices) {
	// Index routes by taxi and precompute each route's request-price penalty.
	MasterResult result;
	result.selected_columns = initial_columns;
	for (auto column_id : initial_columns) {
		if (column_id >= 0) {
			result.profit += columns[column_id].profit;
		}
	}
	vector<IndexVector> taxi_columns(taxi_count);
	for (size_t j = 0; j < columns.size(); ++j) {
		taxi_columns[columns[j].taxi].push_back(j);
	}
	RealVector penalties(columns.size(), 0);
	if (!request_prices.empty()) {
		for (size_t j = 0; j < columns.size(); ++j) {
			for (auto request : columns[j].requests) {
				penalties[j] += request_prices[request];
			}
		}
	}
	// Try several greedy fleet constructions before coordinated group improvement.
	for (Real scale : {0.0, 0.5, 1.0, 1.5}) {
		IndexVector ranking(columns.size());
		iota(ranking.begin(), ranking.end(), 0);
		sort(ranking.begin(), ranking.end(), [&](auto left, auto right) {
			auto left_score = columns[left].profit - scale * penalties[left],
				 right_score = columns[right].profit - scale * penalties[right];
			return left_score != right_score ? left_score > right_score : left < right;
		});
		vector<unsigned char> occupied(request_count, 0);
		IntVector fleet(taxi_count, -1);
		Real profit = 0;
		for (auto j : ranking) {
			auto &column = columns[j];
			if (fleet[column.taxi] >= 0) {
				continue;
			}
			bool available = true;
			for (auto request : column.requests) {
				available &= !occupied[request];
			}
			if (!available) {
				continue;
			}
			fleet[column.taxi] = int(j);
			for (auto request : column.requests) {
				occupied[request] = 1;
			}
			profit += column.profit;
		}
		if (profit > result.profit + IMPROVEMENT_EPSILON) {
			result.profit = profit;
			result.selected_columns = move(fleet);
		}
	}
	// Order each taxi's bank by priced profit for candidate selection below.
	for (auto &taxi_bank : taxi_columns) {
		sort(taxi_bank.begin(), taxi_bank.end(), [&](auto left, auto right) {
			auto left_score = columns[left].profit - penalties[left],
				 right_score = columns[right].profit - penalties[right];
			return left_score != right_score ? left_score > right_score : left < right;
		});
	}
	mt19937_64 group_random_engine(group_seed);
	size_t rounds = 0, last_gain = 0;
	while (Clock::now() < deadline) {
		// Rebuild request ownership from the latest incumbent fleet.
		IntVector request_owner(request_count, -1);
		for (size_t taxi = 0; taxi < taxi_count; ++taxi) {
			if (result.selected_columns[taxi] >= 0) {
				for (auto request : columns[result.selected_columns[taxi]].requests) {
					request_owner[request] = int(taxi);
				}
			}
		}
		// Seed a rotating taxi group with owners blocking one of its attractive routes.
		size_t width = min(taxi_count,
			size_t(GROUP_MIN_TAXIS + GROUP_TAXI_INCREMENT * ((rounds / taxi_count) % GROUP_WIDTH_LEVELS)));
		IndexVector group;
		vector<unsigned char> inside(taxi_count, 0);
		auto add = [&](size_t taxi) {
			if (!inside[taxi]) {
				group.push_back(taxi);
				inside[taxi] = 1;
			}
		};
		auto first = rounds % taxi_count;
		add(first);
		if (!taxi_columns[first].empty()) {
			auto rank = rounds % 3 == 0 ? group_random_engine() %
					min<size_t>(GROUP_RANDOM_SEED_CANDIDATES, taxi_columns[first].size())
										: 0;
			for (auto request : columns[taxi_columns[first][rank]].requests) {
				if (request_owner[request] >= 0 && group.size() < width) {
					add(request_owner[request]);
				}
			}
		}
		// Grow the group toward conflicting owners; occasional random choices diversify it.
		while (group.size() < width) {
			RealVector score(taxi_count, 0);
			for (auto taxi : group) {
				auto previous_profit =
					result.selected_columns[taxi] >= 0 ? columns[result.selected_columns[taxi]].profit : 0;
				auto inspected_columns = min<size_t>(GROUP_OVERLAP_CANDIDATES, taxi_columns[taxi].size());
				for (size_t j = 0; j < inspected_columns; ++j) {
					auto &column = columns[taxi_columns[taxi][j]];
					auto overlap_value = max(1.0, column.profit - previous_profit);
					for (auto request : column.requests) {
						if (request_owner[request] >= 0 && !inside[request_owner[request]]) {
							score[request_owner[request]] += overlap_value;
						}
					}
				}
			}
			size_t chosen = taxi_count;
			Real maximum = 0;
			for (size_t taxi = 0; taxi < taxi_count; ++taxi) {
				if (!inside[taxi] && score[taxi] > maximum) {
					maximum = score[taxi];
					chosen = taxi;
				}
			}
			if (chosen == taxi_count || rounds % 5 == 4) {
				do {
					chosen = group_random_engine() % taxi_count;
				} while (inside[chosen]);
			}
			add(chosen);
		}
		// Keep bounded candidate banks that avoid requests owned outside this group.
		vector<MasterColumn> local_columns;
		IndexVector global_columns;
		IntVector initial(group.size(), -1);
		for (size_t taxi = 0; taxi < group.size(); ++taxi) {
			size_t candidate_count = 0;
			for (auto column_id : taxi_columns[group[taxi]]) {
				auto &column = columns[column_id];
				bool available = true;
				for (auto request : column.requests) {
					if (request_owner[request] >= 0 && !inside[request_owner[request]]) {
						available = false;
						break;
					}
				}
				if (!available) {
					continue;
				}
				// Keep the best 64 available candidates and always retain the incumbent.
				if (candidate_count++ >= GROUP_ROUTE_CANDIDATES &&
					result.selected_columns[group[taxi]] != int(column_id)) {
					continue;
				}
				if (result.selected_columns[group[taxi]] == int(column_id)) {
					initial[taxi] = int(local_columns.size());
				}
				local_columns.push_back({taxi, column.requests, column.profit});
				global_columns.push_back(column_id);
			}
		}
		// Solve the small group master within a short slice of the remaining budget.
		auto current_time = Clock::now();
		auto local_deadline = min(deadline, current_time + Milliseconds(GROUP_SOLVE_MILLISECONDS));
		auto solved = solve_route_master(local_columns, group.size(), request_count, initial, local_deadline);
		Real previous_profit = 0;
		for (auto column_id : initial) {
			if (column_id >= 0) {
				previous_profit += local_columns[column_id].profit;
			}
		}
		// Map improved local choices back to global columns; all outside routes stay fixed.
		if (solved.profit > previous_profit + IMPROVEMENT_EPSILON) {
			for (size_t taxi = 0; taxi < group.size(); ++taxi) {
				result.selected_columns[group[taxi]] = solved.selected_columns[taxi] >= 0
					? int(global_columns[solved.selected_columns[taxi]])
					: -1;
			}
			result.profit += solved.profit - previous_profit;
			last_gain = rounds;
		}
		++rounds;
		if (rounds - last_gain > GROUP_STAGNATION_ROUNDS_PER_TAXI * taxi_count) {
			break;
		}
	}
	result.timed_out = Clock::now() >= deadline;
	return result;
}

// --- Route selection: ranked subsets ---
// Solve progressively wider price-ranked route subsets, then refine taxi groups.
MasterResult solve_ranked_master(const vector<MasterColumn> &columns,
	int taxi_count,
	int request_count,
	const IntVector &initial_columns,
	Deadline deadline) {
	// Estimate opportunity costs from the full LP and rank each taxi's routes.
	MasterResult best;
	best.selected_columns = initial_columns;
	for (int j : initial_columns) {
		if (j >= 0) {
			best.profit += columns[j].profit;
		}
	}
	RealVector prices;
	auto matrix = build_packing_matrix(columns, taxi_count, request_count);
	auto lp = initialize_lp(matrix);
	primal_simplex(lp, min(deadline, deadline_after(RANKED_LP_SECONDS)));
	prices = request_prices_from_lp(lp, request_count);
	vector<IntVector> taxi_columns(taxi_count);
	RealVector score(columns.size());
	for (int j = 0; j < (int)columns.size(); j++) {
		auto &column = columns[j];
		taxi_columns[column.taxi].push_back(j);
		score[j] = column.profit;
		for (int request : column.requests) {
			score[j] -= prices[request];
		}
	}
	for (auto &taxi_bank : taxi_columns) {
		sort(taxi_bank.begin(), taxi_bank.end(), [&](int left, int right) {
			return score[left] != score[right] ? score[left] > score[right] : left < right;
		});
	}
	// Widen the candidate subsets gradually, always retaining the current incumbent.
	for (int width : {32, 64, 128}) {
		if (Clock::now() >= deadline) {
			break;
		}
		vector<MasterColumn> subset_columns;
		IntVector global_columns, initial(taxi_count, -1);
		for (int taxi = 0; taxi < taxi_count; taxi++) {
			auto add = [&](int j) {
				if (best.selected_columns[taxi] == j) {
					initial[taxi] = subset_columns.size();
				}
				subset_columns.push_back(columns[j]);
				global_columns.push_back(j);
			};
			for (int i = 0; i < min<int>(width, taxi_columns[taxi].size()); i++) {
				add(taxi_columns[taxi][i]);
			}
			if (best.selected_columns[taxi] >= 0 && initial[taxi] < 0) {
				add(best.selected_columns[taxi]);
			}
		}
		auto solved = solve_route_master(subset_columns, taxi_count, request_count, initial,
			min(deadline,
				deadline_after(width == 32 ? RANKED_FIRST_SOLVE_SECONDS : RANKED_WIDE_SOLVE_SECONDS)));
		if (solved.profit > best.profit + FEASIBILITY_EPSILON) {
			best.profit = solved.profit;
			for (int taxi = 0; taxi < taxi_count; taxi++) {
				best.selected_columns[taxi] =
					solved.selected_columns[taxi] >= 0 ? global_columns[solved.selected_columns[taxi]] : -1;
			}
		}
	}
	// Spend any remaining time on coordinated changes among taxi groups.
	if (Clock::now() + Milliseconds(RANKED_GROUP_GUARD_MILLISECONDS) < deadline) {
		auto grouped = solve_taxi_groups(
			columns, taxi_count, request_count, best.selected_columns, deadline, random_engine(), prices);
		if (grouped.profit > best.profit + FEASIBILITY_EPSILON) {
			best.profit = grouped.profit;
			best.selected_columns = move(grouped.selected_columns);
		}
	}
	return best;
}

// --- Route selection: request prices ---
// Subgradient request prices penalize collisions between independently chosen taxi routes.
// A feasible fleet profit supplies the lower bound used by the step size.
RealVector estimate_request_prices(Real incumbent_profit) {
	// Collect bank columns and build a feasible greedy profit bound for step-size control.
	RealVector prices(instance.request_count), best_prices(instance.request_count);
	Real lower = 0, best = INF;
	vector<MasterColumn> columns;
	for (int taxi = 0; taxi < instance.taxi_count; taxi++) {
		for (auto &[ids, column] : route_bank[taxi]) {
			columns.push_back({(size_t)taxi, IndexVector(ids.begin(), ids.end()), column.profit});
		}
	}
	IntVector ranking(columns.size());
	iota(ranking.begin(), ranking.end(), 0);
	sort(ranking.begin(), ranking.end(), [&](int left, int right) {
		return columns[left].profit > columns[right].profit;
	});
	vector<char> used_requests(instance.request_count), assigned_taxis(instance.taxi_count);
	for (int j : ranking) {
		auto &column = columns[j];
		if (assigned_taxis[column.taxi]) {
			continue;
		}
		bool available = true;
		for (int request : column.requests) {
			available &= !used_requests[request];
		}
		if (available) {
			lower += column.profit;
			assigned_taxis[column.taxi] = 1;
			for (int request : column.requests) {
				used_requests[request] = 1;
			}
		}
	}
	lower = max(lower, incumbent_profit);
	for (int iteration = 0; iteration < PRICE_ITERATION_LIMIT; iteration++) {
		// Relax shared-request conflicts: let each taxi choose its best priced route independently.
		RealVector best_taxi_score(instance.taxi_count);
		IntVector chosen(instance.taxi_count, -1), request_usage(instance.request_count);
		for (int j = 0; j < (int)columns.size(); j++) {
			auto &column = columns[j];
			Real reduced_profit = column.profit;
			for (int request : column.requests) {
				reduced_profit -= prices[request];
			}
			if (reduced_profit > best_taxi_score[column.taxi]) {
				best_taxi_score[column.taxi] = reduced_profit, chosen[column.taxi] = j;
			}
		}
		// Retain the price vector with the smallest dual bound seen so far.
		Real dual_value = accumulate(prices.begin(), prices.end(), 0.0) +
			accumulate(best_taxi_score.begin(), best_taxi_score.end(), 0.0);
		if (dual_value < best) {
			best = dual_value, best_prices = prices;
		}
		// Count request collisions and compute the squared projected subgradient norm.
		for (int j : chosen) {
			if (j >= 0) {
				for (int request : columns[j].requests) {
					request_usage[request]++;
				}
			}
		}
		Real squared_norm = 0;
		for (int request = 0; request < instance.request_count; request++) {
			if (prices[request] > 0 || request_usage[request] > 1) {
				squared_norm += (request_usage[request] - 1) * (request_usage[request] - 1);
			}
		}
		if (squared_norm == 0 || dual_value <= lower + LP_FEASIBILITY_EPSILON) {
			break;
		}
		// Adjust prices by excess request usage, keeping every price nonnegative.
		Real step = PRICE_STEP_FACTOR * (dual_value - lower) / squared_norm;
		for (int request = 0; request < instance.request_count; request++) {
			prices[request] = max(0.0, prices[request] + step * (request_usage[request] - 1));
		}
	}
	return best_prices;
}

// --- Local search: descent ---
// Descend through removal, insertion, relocation, tail exchange, swap, and replacement.
void improve_solution(SearchState &state, Deadline deadline, bool full_neighborhoods, int cycles) {
	static InsertionCache cache;
	for (int cycle = 0; cycle < cycles && Clock::now() < deadline; cycle++) {
		// Exhaust profitable removals and insertions before trying multi-route changes.
		bool changed = false;
		while (Clock::now() < deadline && improve_by_removal(state)) {
			changed = true;
		}
		while (Clock::now() < deadline && improve_by_insertion(state, cache)) {
			changed = true;
		}
		// Full descent also relocates requests; both modes exchange closed route tails.
		if (full_neighborhoods) {
			while (Clock::now() < deadline && improve_by_relocation(state, deadline)) {
				changed = true;
			}
		}
		while (Clock::now() < deadline && improve_by_tail_exchange(state, deadline)) {
			changed = true;
		}

		// Finish with optional swaps and cached one-for-one replacements.
		if (full_neighborhoods) {
			while (Clock::now() < deadline && improve_by_swap(state, deadline)) {
				changed = true;
			}
		}
		while (Clock::now() < deadline && improve_by_replacement(state, deadline)) {
			changed = true;
		}
		// Repeat the neighborhood sequence only if this cycle changed the fleet.
		if (!changed) {
			break;
		}
	}
	store_fleet(state.routes);
}

// --- Local search: insertion candidates ---
// Filter positions by cost bounds, then check time windows and capacity by segments.
InsertionChoice find_route_insertion(int request, int taxi, const Route &route, Real minimum_profit_delta) {
	// Reject an oversized request and prepare reusable cost-delta arrays.
	InsertionChoice best;
	auto &request_data = instance.requests[request];
	if (request_data.weight > instance.taxi_capacity[taxi] + FEASIBILITY_EPSILON) {
		return best;
	}
	int route_length = route.size();
	static RealVector pickup_cost_delta, delivery_cost_delta, adjacent_cost_delta, minimum_delivery_suffix;
	pickup_cost_delta.resize(route_length + 1);
	delivery_cost_delta.resize(route_length + 1);
	adjacent_cost_delta.resize(route_length + 1);
	minimum_delivery_suffix.assign(route_length + 2, INF);
	// Bound insertion cost before checking time windows: adjacent stops or split parcel stops.
	Real minimum_cost_delta = INF, minimum_pickup_delta = INF;
	for (int pickup_position = 0; pickup_position <= route_length; pickup_position++) {
		int previous_node = pickup_position ? route[pickup_position - 1] : instance.taxi_origin[taxi];
		Real replaced_arc_cost =
			pickup_position < route_length ? travel_cost(previous_node, route[pickup_position]) : 0;
		pickup_cost_delta[pickup_position] = travel_cost(previous_node, request_data.pickup_node) -
			replaced_arc_cost +
			(pickup_position < route_length ? travel_cost(request_data.pickup_node, route[pickup_position])
											: 0);
		delivery_cost_delta[pickup_position] = travel_cost(previous_node, request_data.delivery_node) -
			replaced_arc_cost +
			(pickup_position < route_length ? travel_cost(request_data.delivery_node, route[pickup_position])
											: 0);
		adjacent_cost_delta[pickup_position] = travel_cost(previous_node, request_data.pickup_node) +
			travel_cost(request_data.pickup_node, request_data.delivery_node) - replaced_arc_cost +
			(pickup_position < route_length ? travel_cost(request_data.delivery_node, route[pickup_position])
											: 0);
		minimum_cost_delta = min(minimum_cost_delta, adjacent_cost_delta[pickup_position]);
		if (!request_data.is_passenger) {
			minimum_cost_delta =
				min(minimum_cost_delta, minimum_pickup_delta + delivery_cost_delta[pickup_position]);
			minimum_pickup_delta = min(minimum_pickup_delta, pickup_cost_delta[pickup_position]);
		}
	}
	if (request_data.revenue - minimum_cost_delta + FEASIBILITY_EPSILON < minimum_profit_delta) {
		return best;
	}

	// Suffix minima bound the best delivery position after each possible pickup.
	for (int pickup_position = route_length; pickup_position >= 0; pickup_position--) {
		minimum_delivery_suffix[pickup_position] =
			min(delivery_cost_delta[pickup_position], minimum_delivery_suffix[pickup_position + 1]);
	}
	// Reuse route summaries to check insertions without rebuilding every candidate route.
	auto &segments = cached_route_segments(taxi, route);
	auto pickup = make_node_segment(request_data.pickup_node),
		 delivery = make_node_segment(request_data.delivery_node);
	for (int pickup_position = 0; pickup_position <= route_length; pickup_position++) {
		if (is_insertion_boundary(route, pickup_position)) {
			Real required_profit_delta = max(minimum_profit_delta, best.profit_delta),
				 cost_lower_bound = request_data.is_passenger
				? adjacent_cost_delta[pickup_position]
				: min(adjacent_cost_delta[pickup_position],
					  pickup_cost_delta[pickup_position] + minimum_delivery_suffix[pickup_position + 1]);
			if (request_data.revenue - cost_lower_bound + FEASIBILITY_EPSILON < required_profit_delta) {
				continue;
			}
			// A feasible pickup prefix is required before exploring delivery positions.
			auto prefix = join_segments(segment_range(segments, 0, pickup_position), pickup);
			if (!is_segment_feasible(taxi, prefix)) {
				continue;
			}
			for (int delivery_position = pickup_position;
				delivery_position <= (request_data.is_passenger ? pickup_position : route_length);
				delivery_position++) {
				if (is_insertion_boundary(route, delivery_position)) {
					Real profit_delta = request_data.revenue -
						(pickup_position == delivery_position ? adjacent_cost_delta[pickup_position]
															  : pickup_cost_delta[pickup_position] +
									delivery_cost_delta[delivery_position]);
					if (profit_delta < max(minimum_profit_delta, best.profit_delta) - FEASIBILITY_EPSILON) {
						continue;
					}
					// Assemble prefix, middle, delivery, and suffix summaries for the complete route.
					auto completed_route = join_segments(
						join_segments(join_segments(prefix,
										  segment_range(segments, pickup_position, delivery_position)),
							delivery),
						segment_range(segments, delivery_position, route_length));
					if (is_segment_feasible(taxi, completed_route) &&
						(best.taxi < 0 || profit_delta > best.profit_delta)) {
						best = {request, taxi, pickup_position, delivery_position, profit_delta};
					}
				}
			}
		}
	}
	return best;
}

InsertionChoice find_fleet_insertion(SearchState &state, int request, InsertionCache &cache) {
	if (cache.route_version.empty()) {
		cache.route_version.assign(instance.request_count, vector<UInt64>(instance.taxi_count));
		cache.choices.assign(instance.request_count, vector<InsertionChoice>(instance.taxi_count));
	}
	InsertionChoice best;
	for (int taxi = 0; taxi < instance.taxi_count; taxi++) {
		// Recompute only taxi routes that changed since this request was last examined.
		if (cache.route_version[request][taxi] != state.route_version[taxi]) {
			cache.choices[request][taxi] = find_route_insertion(request, taxi, state.routes[taxi]);
			cache.route_version[request][taxi] = state.route_version[taxi];
		}
		auto choice = cache.choices[request][taxi];
		if (choice.taxi >= 0 && choice.profit_delta > FEASIBILITY_EPSILON &&
			choice.profit_delta > best.profit_delta) {
			best = choice;
		}
	}
	return best;
}

// --- Local search: neighborhoods ---
// Remove one request if the remaining complete route is feasible and more profitable.
bool improve_by_removal(SearchState &state) {
	Real best = FEASIBILITY_EPSILON;
	int removed_request = -1;
	Route route;
	for (int request = 0; request < instance.request_count; request++) {
		if (state.request_owner[request] >= 0) {
			int taxi = state.request_owner[request];
			auto reduced_route = remove_request(state.routes[taxi], request);
			auto evaluated_profit = evaluate_route(instance, taxi, reduced_route);
			if (evaluated_profit && *evaluated_profit - state.route_profit[taxi] > best) {
				best = *evaluated_profit - state.route_profit[taxi];
				removed_request = request;
				route = move(reduced_route);
			}
		}
	}
	if (removed_request < 0) {
		return false;
	}
	update_route(state, state.request_owner[removed_request], route);
	return true;
}

bool improve_by_insertion(SearchState &state, InsertionCache &cache) {
	InsertionChoice best;
	for (int request = 0; request < instance.request_count; request++) {
		if (state.request_owner[request] < 0) {
			auto choice = find_fleet_insertion(state, request, cache);
			if (choice.taxi >= 0 && choice.profit_delta > best.profit_delta) {
				best = choice;
			}
		}
	}
	if (best.taxi < 0) {
		return false;
	}
	apply_insertion(state, best);
	return true;
}

// Replace a served request with an unserved one; cache the best pair per taxi/incoming request.
// A removal-only route may be infeasible on nonmetric input: test the completed move.
bool improve_by_replacement(SearchState &state, Deadline deadline) {
	// Track route versions and unserved requests from the last completed cache refresh.
	auto &cache = replacement_cache;
	if (cache.route_version.empty()) {
		cache.route_version.assign(instance.taxi_count, 0);
		cache.is_unserved.assign(instance.request_count, 0);
		cache.choices.assign(instance.taxi_count, vector<ReplacementChoice>(instance.request_count));
	}
	// Unchanged routes need entries only for requests that have just become unserved.
	IntVector incoming, newly_unserved;
	for (int request = 0; request < instance.request_count; request++) {
		if (state.request_owner[request] < 0) {
			incoming.push_back(request);
			if (!cache.is_unserved[request]) {
				newly_unserved.push_back(request);
			}
		}
	}
	// Refresh affected taxi/request entries by trying each outgoing served request.
	for (int taxi = 0; taxi < instance.taxi_count; taxi++) {
		bool changed = cache.route_version[taxi] != state.route_version[taxi];
		auto &ids = changed ? incoming : newly_unserved;
		if (ids.empty()) {
			continue;
		}
		cache.route_version[taxi] = 0; // Invalidate before a timed refresh.
		for (int request : ids) {
			cache.choices[taxi][request] = ReplacementChoice{};
		}
		for (int outgoing_request : served_requests(state.routes[taxi])) {
			auto reduced_route = remove_request(state.routes[taxi], outgoing_request);
			Real removal_gain = route_profit(taxi, reduced_route) - state.route_profit[taxi];
			for (int incoming_request : ids) {
				if (Clock::now() >= deadline) {
					// The unserved bitmap is committed only after a full refresh.
					// On timeout, invalidate every row before returning.
					fill(cache.route_version.begin(), cache.route_version.end(), 0);
					return false;
				}
				auto &entry = cache.choices[taxi][incoming_request];
				Real required_profit_delta = max(FEASIBILITY_EPSILON, entry.profit_delta);
				auto choice = find_route_insertion(
					incoming_request, taxi, reduced_route, required_profit_delta - removal_gain);
				if (choice.taxi >= 0 && removal_gain + choice.profit_delta > required_profit_delta) {
					entry = {outgoing_request, choice, removal_gain + choice.profit_delta};
				}
			}
		}
		cache.route_version[taxi] = state.route_version[taxi];
	}

	// Commit the unserved bitmap only after every requested cache entry is refreshed.
	for (int request = 0; request < instance.request_count; request++) {
		cache.is_unserved[request] = state.request_owner[request] < 0;
	}

	// Select the best cached move, using request IDs and insertion positions to break ties.
	ReplacementChoice best;
	best.profit_delta = FEASIBILITY_EPSILON;
	for (int taxi = 0; taxi < instance.taxi_count; taxi++) {
		for (int incoming_request : incoming) {
			auto &replacement = cache.choices[taxi][incoming_request];
			if (replacement.outgoing_request >= 0 &&
				(replacement.profit_delta > best.profit_delta ||
					(replacement.profit_delta == best.profit_delta &&
						tie(replacement.outgoing_request, replacement.insertion.request,
							replacement.insertion.pickup_position, replacement.insertion.delivery_position) <
							tie(best.outgoing_request, best.insertion.request, best.insertion.pickup_position,
								best.insertion.delivery_position)))) {
				best = replacement;
			}
		}
	}
	if (best.outgoing_request < 0) {
		return false;
	}
	// Apply removal and insertion together so feasibility belongs to the completed move.
	int taxi = best.insertion.taxi;
	update_route(state, taxi,
		insert_request(remove_request(state.routes[taxi], best.outgoing_request), best.insertion.request,
			best.insertion.pickup_position, best.insertion.delivery_position));
	return true;
}

bool improve_by_tail_exchange(SearchState &state, Deadline deadline) {
	// Only cut where no request is onboard, so exchanged tails keep pickup/delivery pairs intact.
	vector<IntVector> cuts(instance.taxi_count);
	for (int taxi = 0; taxi < instance.taxi_count; taxi++) {
		cuts[taxi] = empty_load_cuts(state.routes[taxi]);
	}
	for (int first_taxi = 0; first_taxi < instance.taxi_count && Clock::now() < deadline; first_taxi++) {
		for (int second_taxi = first_taxi + 1; second_taxi < instance.taxi_count; second_taxi++) {
			auto &first_route = state.routes[first_taxi];
			auto &second_route = state.routes[second_taxi];
			int first_size = first_route.size(), second_size = second_route.size();
			if (first_route.empty() && second_route.empty()) {
				continue;
			}
			Real best = FEASIBILITY_EPSILON;
			int first_cut = -1, second_cut = -1;
			auto &first_segments = cached_route_segments(first_taxi, first_route);
			auto &second_segments = cached_route_segments(second_taxi, second_route);

			// Internal tail costs stay unchanged; only the two connecting arcs affect profit.
			for (int i : cuts[first_taxi]) {
				for (int j : cuts[second_taxi]) {
					int first_previous = i ? first_route[i - 1] : instance.taxi_origin[first_taxi],
						second_previous = j ? second_route[j - 1] : instance.taxi_origin[second_taxi];
					Real profit_delta = 0;
					if (i < first_size) {
						profit_delta += travel_cost(first_previous, first_route[i]) -
							travel_cost(second_previous, first_route[i]);
					}
					if (j < second_size) {
						profit_delta += travel_cost(second_previous, second_route[j]) -
							travel_cost(first_previous, second_route[j]);
					}
					if (profit_delta <= best + FEASIBILITY_EPSILON) {
						continue;
					}
					// Check both recombined routes using cached prefix/suffix summaries.
					if (!is_segment_feasible(first_taxi,
							join_segments(segment_range(first_segments, 0, i),
								segment_range(second_segments, j, second_size))) ||
						!is_segment_feasible(second_taxi,
							join_segments(segment_range(second_segments, 0, j),
								segment_range(first_segments, i, first_size)))) {
						continue;
					}
					best = profit_delta;
					first_cut = i;
					second_cut = j;
				}
			}
			// Apply the best exchange for the first taxi pair that admits an improvement.
			if (first_cut >= 0) {
				Route new_first_route(first_route.begin(), first_route.begin() + first_cut),
					new_second_route(second_route.begin(), second_route.begin() + second_cut);
				new_first_route.insert(
					new_first_route.end(), second_route.begin() + second_cut, second_route.end());
				new_second_route.insert(
					new_second_route.end(), first_route.begin() + first_cut, first_route.end());
				update_route_pair(state, first_taxi, new_first_route, second_taxi, new_second_route);
				return true;
			}
		}
	}
	return false;
}

// Move a request within its route or to another taxi, choosing new insertion positions.
bool improve_by_relocation(SearchState &state, Deadline deadline) {
	Real best = FEASIBILITY_EPSILON;
	int source_taxi = -1, target_taxi = -1;
	Route source_route, target_route;
	// Remove each served request and search its best reinsertion across all taxis.
	for (int request = 0; request < instance.request_count && Clock::now() < deadline; request++) {
		if (state.request_owner[request] >= 0) {
			int taxi = state.request_owner[request];
			auto reduced_route = remove_request(state.routes[taxi], request);
			auto removal_feasible = evaluate_route(instance, taxi, reduced_route);
			Real removal_gain = route_profit(taxi, reduced_route) - state.route_profit[taxi];
			for (int destination = 0; destination < instance.taxi_count; destination++) {
				// A cross-taxi move must leave the source feasible; same-taxi reinsertion can repair it.
				if (destination != taxi && !removal_feasible) {
					continue;
				}
				auto &insertion_route = destination == taxi ? reduced_route : state.routes[destination];
				auto choice =
					find_route_insertion(request, destination, insertion_route, best - removal_gain);
				if (choice.taxi >= 0 && removal_gain + choice.profit_delta > best) {
					best = removal_gain + choice.profit_delta;
					source_taxi = taxi;
					target_taxi = destination;
					source_route = reduced_route;
					target_route = insert_request(
						insertion_route, request, choice.pickup_position, choice.delivery_position);
				}
			}
		}
	}
	if (source_taxi < 0) {
		return false;
	}
	// Update both routes together, including the case where source and destination coincide.
	update_route_pair(state, source_taxi, source_route, target_taxi, target_route);
	return true;
}

// Swap requests between taxis at their existing pickup/delivery positions.
bool improve_by_swap(SearchState &state, Deadline deadline) {
	Real best = FEASIBILITY_EPSILON;
	int best_first_taxi = -1, best_second_taxi = -1;
	Route best_first_route, best_second_route;
	// Examine served request pairs on different taxis and retain the best feasible swap.
	for (int first_request = 0; first_request < instance.request_count && Clock::now() < deadline;
		first_request++) {
		if (state.request_owner[first_request] >= 0) {
			for (int second_request = first_request + 1; second_request < instance.request_count;
				second_request++) {
				if (state.request_owner[second_request] >= 0 &&
					state.request_owner[first_request] != state.request_owner[second_request]) {
					int first_taxi = state.request_owner[first_request],
						second_taxi = state.request_owner[second_request];
					// Reuse the old request's positions; keep incoming passenger stops adjacent.
					auto replace = [&](int taxi, int outgoing_request, int incoming_request) {
						auto route = state.routes[taxi];
						int pickup_position = find(route.begin(), route.end(),
												  instance.requests[outgoing_request].pickup_node) -
							route.begin(),
							delivery_position = find(route.begin(), route.end(),
													instance.requests[outgoing_request].delivery_node) -
							route.begin();
						route.erase(route.begin() + delivery_position);
						route.erase(route.begin() + pickup_position);
						return insert_request(route, incoming_request, pickup_position,
							instance.requests[incoming_request].is_passenger ? pickup_position
																			 : delivery_position - 1);
					};
					auto first_route = replace(first_taxi, first_request, second_request),
						 second_route = replace(second_taxi, second_request, first_request);
					Real profit_delta = route_profit(first_taxi, first_route) +
						route_profit(second_taxi, second_route) - state.route_profit[first_taxi] -
						state.route_profit[second_taxi];
					if (profit_delta <= best) {
						continue;
					}
					// Profit is a cheap filter; accept only when both completed routes are feasible.
					if (evaluate_route(instance, first_taxi, first_route) &&
						evaluate_route(instance, second_taxi, second_route)) {
						best = profit_delta;
						best_first_taxi = first_taxi;
						best_second_taxi = second_taxi;
						best_first_route = move(first_route);
						best_second_route = move(second_route);
					}
				}
			}
		}
	}
	if (best_first_taxi < 0) {
		return false;
	}
	update_route_pair(state, best_first_taxi, best_first_route, best_second_taxi, best_second_route);
	return true;
}

// ============================================================================
// Helper implementations: input, route bookkeeping, bounds, caches, and LP kernels.
// ============================================================================

// --- Helpers: Input and route evaluation ---
Instance read_instance() {
	// Read fleet dimensions and map one-based service nodes back to zero-based requests.
	Instance input;
	cin >> input.taxi_count >> input.passenger_count >> input.parcel_count;
	input.request_count = input.passenger_count + input.parcel_count;
	input.node_count = input.taxi_count + 2 * input.request_count;
	input.taxi_origin.resize(input.taxi_count);
	input.taxi_capacity.resize(input.taxi_count);
	for (int taxi = 0; taxi < input.taxi_count; taxi++) {
		cin >> input.taxi_origin[taxi] >> input.taxi_capacity[taxi];
	}
	input.requests.resize(input.request_count);
	input.node_request.assign(input.node_count + 1, -1);
	input.is_delivery.assign(input.node_count + 1, 0);
	// Passenger records have pickup windows; parcel records also include weight and delivery windows.
	for (int passenger = 0; passenger < input.passenger_count; passenger++) {
		auto &request_data = input.requests[passenger];
		request_data.is_passenger = 1;
		cin >> request_data.pickup_node >> request_data.delivery_node >> request_data.pickup_earliest >>
			request_data.pickup_latest >> request_data.service_time >> request_data.revenue;
		input.node_request[request_data.pickup_node] = input.node_request[request_data.delivery_node] =
			passenger;
		input.is_delivery[request_data.delivery_node] = 1;
	}
	for (int parcel = 0; parcel < input.parcel_count; parcel++) {
		auto &request_data = input.requests[input.passenger_count + parcel];
		request_data.is_passenger = 0;
		cin >> request_data.pickup_node >> request_data.delivery_node >> request_data.weight >>
			request_data.pickup_earliest >> request_data.pickup_latest >> request_data.delivery_earliest >>
			request_data.delivery_latest >> request_data.service_time >> request_data.revenue;
		input.node_request[request_data.pickup_node] = input.node_request[request_data.delivery_node] =
			input.passenger_count + parcel;
		input.is_delivery[request_data.delivery_node] = 1;
	}
	// Store the directed travel matrices in row-major order.
	input.travel_time.resize(input.node_count * input.node_count);
	input.travel_cost.resize(input.node_count * input.node_count);
	for (auto &value : input.travel_time) {
		cin >> value;
	}
	for (auto &value : input.travel_cost) {
		cin >> value;
	}
	return input;
}

// Return profit for a feasible route, or no value when a route constraint fails.
// Passengers require adjacent pickup/delivery; parcels may stay onboard between stops.
optional<Real> evaluate_route(const Instance &input, int taxi, const IntVector &route) {
	// Visit stamps avoid clearing per-request pickup/delivery markers on every evaluation.
	static vector<UInt64> picked_at, delivered_at;
	static UInt64 evaluation_stamp = 0;
	if (picked_at.size() != (size_t)input.request_count) {
		picked_at.assign(input.request_count, 0);
		delivered_at.assign(input.request_count, 0);
	}
	++evaluation_stamp;
	Real departure_time = 0, load = 0, total_cost = 0, revenue = 0;
	int previous_node = input.taxi_origin[taxi], open_requests = 0;
	// Simulate stops in order, accumulating arc cost and checking time, load, and precedence.
	for (int position = 0; position < (int)route.size(); ++position) {
		int node = route[position], request = input.node_request[node];
		auto &request_data = input.requests[request];
		Real arrival = departure_time + input.travel_time[(previous_node - 1) * input.node_count + node - 1];
		if (!isfinite(arrival)) {
			return {};
		}
		total_cost += input.travel_cost[(previous_node - 1) * input.node_count + node - 1];
		if (!input.is_delivery[node]) {
			if (picked_at[request] == evaluation_stamp ||
				arrival > request_data.pickup_latest + FEASIBILITY_EPSILON) {
				return {};
			}
			if (request_data.is_passenger &&
				(position + 1 == (int)route.size() || route[position + 1] != request_data.delivery_node)) {
				return {};
			}
			load += request_data.weight;
			if (load > input.taxi_capacity[taxi] + FEASIBILITY_EPSILON) {
				return {};
			}
			picked_at[request] = evaluation_stamp;
			++open_requests;
			revenue += request_data.revenue;
			departure_time = max(arrival, request_data.pickup_earliest) + request_data.service_time;
		} else {
			if (picked_at[request] != evaluation_stamp || delivered_at[request] == evaluation_stamp ||
				(!request_data.is_passenger &&
					arrival > request_data.delivery_latest + FEASIBILITY_EPSILON)) {
				return {};
			}
			load -= request_data.weight;
			delivered_at[request] = evaluation_stamp;
			--open_requests;
			departure_time = max(arrival, request_data.delivery_earliest) + request_data.service_time;
		}
		previous_node = node;
	}
	// Revenue counts at pickup; all picked-up requests must be delivered before the route ends.
	if (open_requests) {
		return {};
	}
	return revenue - total_cost;
}

// --- Helpers: Time, travel, and route accounting ---
Deadline deadline_after(Real seconds) {
	return Clock::now() + chrono::duration_cast<Clock::duration>(chrono::duration<Real>(seconds));
}

Real travel_time(int from_node, int to_node) {
	return instance.travel_time[(from_node - 1) * instance.node_count + to_node - 1];
}

Real travel_cost(int from_node, int to_node) {
	return instance.travel_cost[(from_node - 1) * instance.node_count + to_node - 1];
}

IntVector served_requests(const Route &route) {
	IntVector requests;
	for (int node : route) {
		if (!instance.is_delivery[node]) {
			requests.push_back(instance.node_request[node]);
		}
	}
	sort(requests.begin(), requests.end());
	return requests;
}

// Arc cost only; the route need not be feasible.
Real route_cost(int taxi, const Route &route) {
	Real total_cost = 0;
	int previous_node = instance.taxi_origin[taxi];
	for (int node : route) {
		total_cost += travel_cost(previous_node, node);
		previous_node = node;
	}
	return total_cost;
}

// Revenue minus arc cost; feasibility is checked by the caller when required.
Real route_profit(int taxi, const Route &route) {
	Real total_profit = -route_cost(taxi, route);
	for (int node : route) {
		if (!instance.is_delivery[node]) {
			total_profit += instance.requests[instance.node_request[node]].revenue;
		}
	}
	return total_profit;
}

Real fleet_profit(const Fleet &fleet) {
	Real total_profit = 0;
	for (int taxi = 0; taxi < instance.taxi_count; taxi++) {
		total_profit += route_profit(taxi, fleet[taxi]);
	}
	return total_profit;
}

// --- Helpers: Route-bank maintenance ---
// Ordinary DFS routes obey the bank cap; fleet and priced routes bypass it.
void store_route(int taxi, const Route &route, bool enforce_limit, Real profit) {
	// Discard unprofitable routes and candidates below a full capped bank's profit floor.
	if (profit == -INF) {
		profit = route_profit(taxi, route);
	}
	if (profit <= FEASIBILITY_EPSILON) {
		return;
	}
	auto &taxi_bank = route_bank[taxi];
	if (enforce_limit && taxi_bank.size() >= (size_t)bank_route_limit && profit < bank_profit_floor[taxi]) {
		return;
	}
	// Routes covering the same request set compete for one bank entry, keeping the best profit.
	auto requests = served_requests(route);
	auto entry = taxi_bank.lower_bound(requests);
	if (entry != taxi_bank.end() && entry->first == requests) {
		if (entry->second.profit >= profit) {
			return;
		}
		entry->second = {route, profit};
	} else {
		taxi_bank.emplace_hint(entry, requests, StoredRoute{route, profit});
	}
	// Protected insertions bypass the cap; capped insertions evict the least profitable route.
	if (!enforce_limit) {
		bank_profit_floor[taxi] = -INF;
	}
	if (enforce_limit && taxi_bank.size() > (size_t)bank_route_limit) {
		auto worst = min_element(taxi_bank.begin(), taxi_bank.end(), [](auto &left, auto &right) {
			return left.second.profit < right.second.profit;
		});
		taxi_bank.erase(worst);
		bank_profit_floor[taxi] =
			min_element(taxi_bank.begin(), taxi_bank.end(), [](auto &left, auto &right) {
				return left.second.profit < right.second.profit;
			})->second.profit;
	}
}

void store_fleet(const Fleet &fleet) {
	for (int taxi = 0; taxi < instance.taxi_count; taxi++) {
		store_route(taxi, fleet[taxi]);
	}
}

// --- Helpers: Fleet-state updates ---
SearchState make_search_state(const Fleet &fleet) {
	SearchState state;
	state.routes = fleet;
	state.request_owner.assign(instance.request_count, -1);
	state.route_profit.resize(instance.taxi_count);
	state.route_version.resize(instance.taxi_count);
	for (int taxi = 0; taxi < instance.taxi_count; taxi++) {
		for (int request : served_requests(fleet[taxi])) {
			state.request_owner[request] = taxi;
		}
		state.route_profit[taxi] = route_profit(taxi, fleet[taxi]);
		state.profit += state.route_profit[taxi];
		state.route_version[taxi] = next_route_version++;
	}
	return state;
}

void update_route(SearchState &state, int taxi, const Route &route) {
	for (int request : served_requests(state.routes[taxi])) {
		state.request_owner[request] = -1;
	}
	state.profit -= state.route_profit[taxi];
	state.routes[taxi] = route;
	state.route_profit[taxi] = route_profit(taxi, route);
	state.profit += state.route_profit[taxi];
	for (int request : served_requests(route)) {
		state.request_owner[request] = taxi;
	}
	state.route_version[taxi] = next_route_version++;
}

void update_route_pair(SearchState &state,
	int first_taxi,
	const Route &first_route,
	int second_taxi,
	const Route &second_route) {
	if (first_taxi == second_taxi) {
		update_route(state, first_taxi, second_route);
		return;
	}
	// Release both old request sets before assigning either new route, avoiding ownership conflicts.
	for (int taxi : {first_taxi, second_taxi}) {
		for (int request : served_requests(state.routes[taxi])) {
			state.request_owner[request] = -1;
		}
		state.profit -= state.route_profit[taxi];
	}
	state.routes[first_taxi] = first_route;
	state.routes[second_taxi] = second_route;
	// Rebuild ownership, profit, and cache versions for both new routes.
	for (int taxi : {first_taxi, second_taxi}) {
		state.route_profit[taxi] = route_profit(taxi, state.routes[taxi]);
		state.profit += state.route_profit[taxi];
		for (int request : served_requests(state.routes[taxi])) {
			state.request_owner[request] = taxi;
		}
		state.route_version[taxi] = next_route_version++;
	}
}

void apply_insertion(SearchState &state, InsertionChoice choice) {
	update_route(state, choice.taxi,
		insert_request(
			state.routes[choice.taxi], choice.request, choice.pickup_position, choice.delivery_position));
	store_route(choice.taxi, state.routes[choice.taxi]);
}

// --- Helpers: Route edits and cut positions ---
// Never split a passenger's adjacent pickup/delivery block.
bool is_insertion_boundary(const Route &route, int position) {
	return !position || instance.is_delivery[route[position - 1]] ||
		!instance.requests[instance.node_request[route[position - 1]]].is_passenger;
}

Route remove_request(const Route &route, int request) {
	Route reduced_route;
	for (int node : route) {
		if (instance.node_request[node] != request) {
			reduced_route.push_back(node);
		}
	}
	return reduced_route;
}

Route insert_request(Route route, int request, int pickup_position, int delivery_position) {
	route.insert(route.begin() + delivery_position, instance.requests[request].delivery_node);
	route.insert(route.begin() + pickup_position, instance.requests[request].pickup_node);
	return route;
}

// Tail exchanges cut only where every picked-up request has been delivered.
IntVector empty_load_cuts(const Route &route) {
	IntVector cuts{0};
	int open_requests = 0;
	for (int i = 0; i < (int)route.size(); i++) {
		open_requests += instance.is_delivery[route[i]] ? -1 : 1;
		if (!open_requests) {
			cuts.push_back(i + 1);
		}
	}
	return cuts;
}

// --- Helpers: Directed delivery bounds ---
// Landmark shortest paths bound parcel delivery time on directed, nonmetric input.
// A direct arc is not a valid lower bound when intermediate stops can be faster.
void build_delivery_lower_bounds(Deadline deadline) {
	int node_count = instance.node_count;
	vector<RealVector> landmarks;
	RealVector spread(node_count, INF);
	vector<char> used(node_count);
	int landmark = 0;
	auto bound_deadline = min(deadline,
		deadline_after(min(LANDMARK_MAX_SECONDS,
			LANDMARK_BUDGET_FRACTION * chrono::duration<Real>(deadline - Clock::now()).count())));
	// Run a bounded number of dense Dijkstra searches from spread-out landmarks.
	for (int pass = 0; pass < min(LANDMARK_LIMIT, node_count) && Clock::now() < bound_deadline; pass++) {
		RealVector distances(instance.travel_time.begin() + landmark * node_count,
			instance.travel_time.begin() + (landmark + 1) * node_count);
		vector<char> visited(node_count);
		distances[landmark] = 0;
		bool complete = true;
		for (int step = 0; step < node_count; step++) {
			if (step % LANDMARK_CLOCK_INTERVAL == 0 && Clock::now() >= bound_deadline) {
				complete = false;
				break;
			}
			int closest_node = -1;
			Real closest_distance = INF;
			for (int node = 0; node < node_count; node++) {
				if (!visited[node] && distances[node] < closest_distance) {
					closest_distance = distances[node];
					closest_node = node;
				}
			}
			if (closest_node < 0) {
				break;
			}
			visited[closest_node] = 1;
			for (int node = 0; node < node_count; node++) {
				if (!visited[node]) {
					distances[node] = min(distances[node],
						closest_distance + instance.travel_time[closest_node * node_count + node]);
				}
			}
		}
		// Discard interrupted searches; only completed shortest-path distances define valid bounds.
		if (!complete) {
			break;
		}
		used[landmark] = 1;
		for (int node = 0; node < node_count; node++) {
			spread[node] = min(spread[node], distances[node]);
		}
		landmarks.push_back(move(distances));
		// Pick the next landmark farthest from its nearest existing landmark.
		Real farthest = -1;
		for (int node = 0; node < node_count; node++) {
			if (!used[node] && spread[node] > farthest) {
				farthest = spread[node];
				landmark = node;
			}
		}
	}
	// Directed triangle inequality for shortest paths gives d(L, delivery) - d(L, node).
	// Reduce by a rounding margin and retain the strongest nonnegative landmark bound.
	delivery_time_lower_bound.assign(node_count * instance.parcel_count, 0);
	for (int node = 0; node < node_count; node++) {
		for (int parcel = 0; parcel < instance.parcel_count; parcel++) {
			for (auto &distances : landmarks) {
				int delivery_node = instance.requests[instance.passenger_count + parcel].delivery_node - 1;
				Real rounding_margin = FEASIBILITY_EPSILON +
					64 * numeric_limits<Real>::epsilon() * node_count *
						max({1.0, distances[delivery_node], distances[node]});
				auto &lower_bound = delivery_time_lower_bound[node * instance.parcel_count + parcel];
				lower_bound = max(lower_bound, distances[delivery_node] - distances[node] - rounding_margin);
			}
		}
	}
}

// --- Helpers: Segment composition and cache queries ---
Segment make_node_segment(int node) {
	auto &request_data = instance.requests[instance.node_request[node]];
	bool is_delivery = instance.is_delivery[node];
	Segment segment;
	segment.first_node = segment.last_node = node;
	segment.duration = request_data.service_time;
	segment.earliest_departure =
		(!is_delivery ? request_data.pickup_earliest : request_data.delivery_earliest) +
		request_data.service_time;
	segment.latest_arrival =
		(!is_delivery ? request_data.pickup_latest : request_data.delivery_latest) + FEASIBILITY_EPSILON;
	if (is_delivery && request_data.is_passenger) {
		segment.earliest_departure = -INF;
		segment.latest_arrival = numeric_limits<Real>::max();
	}
	segment.load_change = is_delivery ? -request_data.weight : request_data.weight;
	segment.minimum_load = min(0.0, segment.load_change);
	segment.maximum_load = max(0.0, segment.load_change);
	return segment;
}

Segment join_segments(Segment left, const Segment &right) {
	if (!left.first_node) {
		return right;
	}
	if (!right.first_node) {
		return left;
	}
	Real arc_time = travel_time(left.last_node, right.first_node), duration = left.duration,
		 earliest_departure = left.earliest_departure, load_change = left.load_change;
	// Compose waiting and latest-arrival limits across the connecting arc.
	left.feasible = left.feasible && right.feasible && earliest_departure + arc_time <= right.latest_arrival;
	left.latest_arrival = min(left.latest_arrival, right.latest_arrival - arc_time - duration);
	left.duration = duration + arc_time + right.duration;
	left.earliest_departure = max(earliest_departure + arc_time + right.duration, right.earliest_departure);
	// Shift right-segment load extrema by the net load change in the left segment.
	left.minimum_load = min(left.minimum_load, load_change + right.minimum_load);
	left.maximum_load = max(left.maximum_load, load_change + right.maximum_load);
	left.load_change = load_change + right.load_change;
	left.last_node = right.last_node;
	return left;
}

bool is_segment_feasible(int taxi, const Segment &segment) {
	return !segment.first_node ||
		(segment.feasible &&
			travel_time(instance.taxi_origin[taxi], segment.first_node) <= segment.latest_arrival &&
			segment.minimum_load >= -FEASIBILITY_EPSILON &&
			segment.maximum_load <= instance.taxi_capacity[taxi] + FEASIBILITY_EPSILON);
}

RouteSegmentCache &cached_route_segments(int taxi, const Route &route) {
	if (route_segment_cache.empty()) {
		route_segment_cache.resize(instance.taxi_count);
	}
	auto &segments = route_segment_cache[taxi];
	if (segments.nodes != route) {
		// Rebuild only when route contents change; first summarize each service node.
		segments.nodes = route;
		segments.node_segments.clear();
		segments.range_table.clear();
		int node_count = route.size();
		for (int node : route) {
			segments.node_segments.push_back(make_node_segment(node));
		}
		// Each level stores suffixes left of a block midpoint and prefixes to its right.
		for (int half = 1; half < node_count; half *= 2) {
			segments.range_table.emplace_back(node_count);
			auto &level = segments.range_table.back();
			for (int start = 0; start < node_count; start += 2 * half) {
				int middle = min(start + half, node_count), end = min(start + 2 * half, node_count);
				if (middle > start) {
					level[middle - 1] = segments.node_segments[middle - 1];
					for (int i = middle - 1; i > start; i--) {
						level[i - 1] = join_segments(segments.node_segments[i - 1], level[i]);
					}
				}
				if (middle < end) {
					level[middle] = segments.node_segments[middle];
					for (int i = middle + 1; i < end; i++) {
						level[i] = join_segments(level[i - 1], segments.node_segments[i]);
					}
				}
			}
		}
	}
	return segments;
}

// Query the half-open range [begin, end).
Segment segment_range(const RouteSegmentCache &segments, int begin, int end) {
	if (begin >= end) {
		return {};
	}
	if (begin + 1 == end) {
		return segments.node_segments[begin];
	}
	// The highest differing endpoint bit selects two disjoint summaries covering the range.
	int level = 31 - __builtin_clz((unsigned)(begin ^ (end - 1)));
	return join_segments(segments.range_table[level][begin], segments.range_table[level][end - 1]);
}

// --- Helpers: Packing matrix and simplex kernels ---
// Rows enforce one route per taxi and at most one visit per shared request.
// Requests appearing only in one taxi's columns need no separate resource row.
PackingMatrix build_packing_matrix(
	const vector<MasterColumn> &columns, size_t taxi_count, size_t request_count) {
	PackingMatrix matrix;
	matrix.columns = &columns;
	matrix.column_rows.resize(columns.size());
	matrix.taxi_count = taxi_count;
	// Mark requests shared across taxis; private requests are already limited by their taxi row.
	IntVector request_taxi(request_count, -1);
	for (auto &column : columns) {
		for (auto resource : column.requests) {
			if (request_taxi[resource] == -1) {
				request_taxi[resource] = column.taxi;
			} else if (request_taxi[resource] != (int)column.taxi) {
				request_taxi[resource] = -2;
			}
		}
	}
	// Compress taxi and shared-request resources into the rows touched by each column.
	IntVector resource_row(taxi_count + request_count, -1);
	for (size_t j = 0; j < columns.size(); ++j) {
		auto add_resource = [&](size_t resource) {
			if (resource >= taxi_count && request_taxi[resource - taxi_count] != -2) {
				return;
			}
			if (resource_row[resource] < 0) {
				resource_row[resource] = int(matrix.dimension++);
				matrix.row_resources.push_back(resource);
			}
			matrix.column_rows[j].push_back(resource_row[resource]);
		};
		add_resource(columns[j].taxi);
		for (auto resource : columns[j].requests) {
			add_resource(taxi_count + resource);
		}
	}
	return matrix;
}

PackingLP initialize_lp(const PackingMatrix &matrix) {
	PackingLP lp;
	lp.matrix = &matrix;
	lp.row_count = matrix.dimension;
	lp.basis_inverse.assign(lp.row_count * lp.row_count, 0);
	lp.basic_values.assign(lp.row_count, 1);
	lp.dual_prices.assign(lp.row_count, 0);
	lp.right_hand_sides.assign(lp.row_count, 1);
	lp.bound_signs.assign(lp.row_count, 0);
	lp.basis_columns.resize(lp.row_count);
	lp.in_basis.assign(matrix.columns->size() + lp.row_count, 0);
	lp.column_bound_rows.assign(matrix.columns->size(), -1);
	// Start with unit slacks as a feasible identity basis; route variables are initially zero.
	for (size_t i = 0; i < lp.row_count; ++i) {
		lp.basis_inverse[i * lp.row_count + i] = 1;
		lp.basis_columns[i] = matrix.columns->size() + i;
		lp.in_basis[lp.basis_columns[i]] = 1;
	}
	return lp;
}

Real column_coefficient(const PackingLP &lp, size_t row, size_t column) {
	auto column_count = lp.matrix->columns->size();
	if (column >= column_count) {
		return lp.basis_inverse[row * lp.row_count + column - column_count];
	}
	Real value = 0;
	for (auto resource_row : lp.matrix->column_rows[column]) {
		value += lp.basis_inverse[row * lp.row_count + resource_row];
	}
	if (lp.column_bound_rows[column] >= 0) {
		auto resource_row = size_t(lp.column_bound_rows[column]);
		value += lp.bound_signs[resource_row] * lp.basis_inverse[row * lp.row_count + resource_row];
	}
	return value;
}

Real reduced_cost(const PackingLP &lp, size_t column) {
	auto column_count = lp.matrix->columns->size();
	if (column >= column_count) {
		return -lp.dual_prices[column - column_count];
	}
	Real value = (*lp.matrix->columns)[column].profit;
	for (auto resource_row : lp.matrix->column_rows[column]) {
		value -= lp.dual_prices[resource_row];
	}
	if (lp.column_bound_rows[column] >= 0) {
		auto resource_row = size_t(lp.column_bound_rows[column]);
		value -= lp.bound_signs[resource_row] * lp.dual_prices[resource_row];
	}
	return value;
}

void pivot_basis(PackingLP &lp, size_t leaving, size_t entering) {
	// Transform the entering column through the current basis inverse.
	RealVector pivot_column(lp.row_count), pivot_row(lp.row_count);
	for (size_t i = 0; i < lp.row_count; ++i) {
		pivot_column[i] = column_coefficient(lp, i, entering);
	}
	auto divisor = pivot_column[leaving];
	Real step = lp.basic_values[leaving] / divisor, reduced_gain = reduced_cost(lp, entering);
	// Normalize the leaving row and use it to update resource dual prices.
	for (size_t resource_row = 0; resource_row < lp.row_count; ++resource_row) {
		pivot_row[resource_row] = lp.basis_inverse[leaving * lp.row_count + resource_row] / divisor;
		lp.dual_prices[resource_row] += reduced_gain * pivot_row[resource_row];
	}
	// Eliminate the pivot column from other rows and update their basic values.
	for (size_t i = 0; i < lp.row_count; ++i) {
		if (i != leaving) {
			lp.basic_values[i] -= step * pivot_column[i];
			if (pivot_column[i] == 0) {
				continue;
			}
			for (size_t resource_row = 0; resource_row < lp.row_count; ++resource_row) {
				lp.basis_inverse[i * lp.row_count + resource_row] -=
					pivot_column[i] * pivot_row[resource_row];
			}
		}
	}
	// Install the new basis row and mark the entering/leaving variables.
	copy(pivot_row.begin(), pivot_row.end(), lp.basis_inverse.begin() + leaving * lp.row_count);
	lp.basic_values[leaving] = step;
	lp.in_basis[lp.basis_columns[leaving]] = 0;
	lp.basis_columns[leaving] = entering;
	lp.in_basis[entering] = 1;
}

void primal_simplex(PackingLP &lp, Deadline deadline) {
	auto column_count = lp.matrix->columns->size();
	size_t degenerate = 0;
	for (size_t iteration = 0; iteration < SIMPLEX_ITERATION_LIMIT; ++iteration) {
		if ((iteration & SIMPLEX_CLOCK_MASK) == 0 && Clock::now() >= deadline) {
			lp.timed_out = true;
			return;
		}
		// Choose a positive reduced-cost column; after repeated zero steps, take the first improving one.
		size_t entering = column_count + lp.row_count;
		Real best_reduced_cost = FEASIBILITY_EPSILON;
		for (size_t j = 0; j < column_count + lp.row_count; ++j) {
			if (lp.in_basis[j]) {
				continue;
			}
			Real value = reduced_cost(lp, j);
			if (value > best_reduced_cost) {
				best_reduced_cost = value;
				entering = j;
				if (degenerate > lp.row_count) {
					break;
				}
			}
		}
		if (entering == column_count + lp.row_count) {
			return;
		}
		// A ratio test limits the step to keep every basic variable nonnegative.
		size_t leaving = lp.row_count;
		Real step = INF;
		for (size_t i = 0; i < lp.row_count; ++i) {
			auto coefficient = column_coefficient(lp, i, entering);
			if (coefficient <= PIVOT_EPSILON) {
				continue;
			}
			auto ratio = max(0.0, lp.basic_values[i]) / coefficient;
			if (ratio < step - PIVOT_EPSILON ||
				(abs(ratio - step) <= PIVOT_EPSILON &&
					(leaving == lp.row_count || lp.basis_columns[i] < lp.basis_columns[leaving]))) {
				leaving = i;
				step = ratio;
			}
		}
		if (leaving == lp.row_count || !isfinite(step)) {
			return;
		}
		// Record zero-length steps and pivot into the chosen adjacent basis.
		degenerate = step < PIVOT_EPSILON ? degenerate + 1 : 0;
		pivot_basis(lp, leaving, entering);
	}
}

// Include/exclude a column with -x <= -1 or x <= 0, retaining the current basis.
void add_column_bound(PackingLP &lp, size_t column, bool include) {
	// Append a signed variable bound and enlarge the basis inverse by one slack row.
	Real sign = include ? -1 : 1, rhs = include ? -1 : 0;
	auto old_dimension = lp.row_count++;
	RealVector enlarged(lp.row_count * lp.row_count, 0);
	for (size_t i = 0; i < old_dimension; ++i) {
		copy_n(
			lp.basis_inverse.begin() + i * old_dimension, old_dimension, enlarged.begin() + i * lp.row_count);
	}
	// If the route variable is basic, eliminate it from the new row using its basis row.
	Real value = 0;
	for (size_t i = 0; i < old_dimension; ++i) {
		if (lp.basis_columns[i] == column) {
			value = lp.basic_values[i];
			for (size_t resource_row = 0; resource_row < old_dimension; ++resource_row) {
				enlarged[old_dimension * lp.row_count + resource_row] =
					-sign * lp.basis_inverse[i * old_dimension + resource_row];
			}
			break;
		}
	}
	enlarged[old_dimension * lp.row_count + old_dimension] = 1;
	lp.basis_inverse = move(enlarged);
	lp.basic_values.push_back(rhs - sign * value);
	lp.dual_prices.push_back(0);
	lp.right_hand_sides.push_back(rhs);
	lp.bound_signs.push_back(sign);
	lp.basis_columns.push_back(lp.matrix->columns->size() + old_dimension);
	lp.in_basis.push_back(1);
	lp.column_bound_rows[column] = int(old_dimension);
}

// Restore primal feasibility after a branch adds a bound.
void dual_simplex(PackingLP &lp, Deadline deadline) {
	auto column_count = lp.matrix->columns->size();
	for (size_t iteration = 0; iteration < SIMPLEX_ITERATION_LIMIT; ++iteration) {
		if ((iteration & SIMPLEX_CLOCK_MASK) == 0 && Clock::now() >= deadline) {
			lp.timed_out = true;
			return;
		}
		// Repair the most negative basic value introduced by an include/exclude constraint.
		size_t leaving = lp.row_count;
		Real minimum = -LP_FEASIBILITY_EPSILON;
		for (size_t i = 0; i < lp.row_count; ++i) {
			if (lp.basic_values[i] < minimum) {
				minimum = lp.basic_values[i];
				leaving = i;
			}
		}
		if (leaving == lp.row_count) {
			return;
		}
		// Choose an eligible entering column by the dual ratio test to preserve dual feasibility.
		size_t entering = column_count + lp.row_count;
		Real ratio = INF;
		for (size_t j = 0; j < column_count + lp.row_count; ++j) {
			if (lp.in_basis[j]) {
				continue;
			}
			auto coefficient = column_coefficient(lp, leaving, j);
			if (coefficient >= -PIVOT_EPSILON) {
				continue;
			}
			Real candidate = reduced_cost(lp, j) / coefficient;
			if (candidate < ratio - PIVOT_EPSILON) {
				entering = j;
				ratio = candidate;
			}
		}
		if (entering == column_count + lp.row_count) {
			return;
		}
		pivot_basis(lp, leaving, entering);
	}
}

RealVector primal_values(const PackingLP &lp) {
	RealVector values(lp.matrix->columns->size(), 0);
	for (size_t i = 0; i < lp.row_count; ++i) {
		if (lp.basis_columns[i] < values.size() && isfinite(lp.basic_values[i])) {
			values[lp.basis_columns[i]] = clamp(lp.basic_values[i], 0.0, 1.0);
		}
	}
	return values;
}

// Correct dual residuals per taxi and add a rounding margin for safe pruning.
Real dual_upper_bound(const PackingLP &lp) {
	// Nonnegative resource prices contribute the main dual bound, including branch rows.
	auto prices = lp.dual_prices;
	Real bound = 0, magnitude = 0;
	for (size_t resource_row = 0; resource_row < lp.row_count; ++resource_row) {
		if (!isfinite(prices[resource_row])) {
			return INF;
		}
		prices[resource_row] = max(0.0, prices[resource_row]);
		bound += prices[resource_row] * lp.right_hand_sides[resource_row];
		magnitude += abs(prices[resource_row] * lp.right_hand_sides[resource_row]);
	}
	// Add each taxi's largest positive reduced-profit residual to cover dual violations.
	RealVector residual(lp.matrix->taxi_count, 0);
	for (size_t j = 0; j < lp.matrix->columns->size(); ++j) {
		auto &column = (*lp.matrix->columns)[j];
		Real value = column.profit;
		for (auto resource_row : lp.matrix->column_rows[j]) {
			value -= prices[resource_row];
		}
		if (lp.column_bound_rows[j] >= 0) {
			auto resource_row = size_t(lp.column_bound_rows[j]);
			value -= lp.bound_signs[resource_row] * prices[resource_row];
		}
		residual[column.taxi] = max(residual[column.taxi], value);
	}
	// Include residuals and numerical margins before using the bound for branch pruning.
	auto remainder = accumulate(residual.begin(), residual.end(), 0.0);
	bound += remainder;
	magnitude += remainder;
	if (!isfinite(bound) || !isfinite(magnitude)) {
		return INF;
	}
	return bound + LP_FEASIBILITY_EPSILON + BOUND_RELATIVE_EPSILON * magnitude;
}

RealVector request_prices_from_lp(const PackingLP &lp, size_t request_count) {
	RealVector result(request_count, 0);
	for (size_t row = 0; row < lp.matrix->row_resources.size(); ++row) {
		auto resource = lp.matrix->row_resources[row];
		if (resource >= lp.matrix->taxi_count && isfinite(lp.dual_prices[row])) {
			result[resource - lp.matrix->taxi_count] = max(0.0, lp.dual_prices[row]);
		}
	}
	return result;
}
