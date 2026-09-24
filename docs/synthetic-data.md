# SOICT 2026 synthetic data

The [contest description](Problem%20description.pdf) gives no file format or instance generator. These are our synthetic development instances in a versioned internal format. They borrow the passenger/parcel mix and pickup-time pattern from [Nguyen et al. (2019)](paper/nguyen2019.pdf); they do **not** reproduce the paper's SF-day instances.

## Generate and inspect

```sh
cmake -S . -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build cmake-build-debug
ctest --test-dir cmake-build-debug --output-on-failure
./cmake-build-debug/SoICT2026 generate --passengers 50 --parcels 50 --taxis 100 --seed 1 --out case.txt
./cmake-build-debug/SoICT2026 inspect case.txt
```

## Fixed test set

Run `bash scripts/generate_synthetic_data.sh` from the repository root after building. It writes 30 instances and `data/synthetic/manifest.csv`. The script can recreate the files; the manifest records their parameters.

| Total requests `N+M` | Instances | Passenger counts `N` (seeds 1–5 in order) | Taxis `K` |
| ---: | ---: | --- | ---: |
| 10 | 5 | 0, 2, 5, 8, 10 | 100 |
| 20 | 5 | 0, 5, 10, 15, 20 | 100 |
| 50 | 5 | 0, 12, 25, 38, 50 | 100 |
| 100 | 5 | 0, 25, 50, 75, 100 | 100 |
| 250 | 5 | 0, 62, 125, 188, 250 | 100 |
| 500 | 5 | 0, 125, 250, 375, 500 | 100 |

For every row, `M = N+M-N`. The five mixes target 0%, 25%, 50%, 75%, and 100% passengers; quarter counts are rounded down and mirrored so they sum correctly. The paper's experiment used 100 taxis, but did not define a taxi-count scaling rule. Holding `K=100` is our test-set choice. At the smallest sizes, many taxis will be unused. The five cases at each size change both mix and seed; they are coverage cases, not repeated trials of one mix. No 1,000-request instances are generated in this set.

## Generation rule

The inputs are taxi count `K`, passenger count `N`, parcel count `M`, and a `uint64` seed (defaults: `100`, `50`, `50`, and `1`). The generator accepts `1 <= K <= 1000` and `1 <= N+M <= 5000`. Counts are explicit: choose `N = M` to use the paper's approximate half-passenger, half-parcel mix. All draws use one `std::mt19937_64` engine. A fixed seed reproduces the instance with the same C++ implementation and standard library; identical output across different standard libraries is not guaranteed.

For each of the `V = K + 2N + 2M` labeled nodes, sample a point `(x,y)` with each coordinate independently uniform on `[0,100]`. For every ordered pair of distinct nodes `(u,v)`, let `d(u,v)` be their Euclidean distance. Generate travel time `t(u,v) = d(u,v) * U(0.9,1.1)` and travel cost `c(u,v) = d(u,v) * U(0.9,1.1)`, drawing a separate factor for each value and direction. Both diagonal entries are zero. The coordinates are only a device for generating the two dense directed matrices; they are not stored in the instance.

Time is measured in minutes. The distributions below are independent draws from the same seeded engine. `direct_time` and `direct_cost` mean the generated matrix entries from that request's pickup to its drop-off; `a = t0 - 10`.

| Field | Passenger request | Parcel request |
| --- | --- | --- |
| Hypothetical pickup time | `t0 ~ U(10,480)` | `t0 ~ U(10,480)` |
| Pickup window | `[t0-10, t0+15]` | `[t0-10, t0+60]` |
| Delivery window | None | `[a+direct_time, a+direct_time+30]` |
| Service duration | `U(1,5)` at each endpoint | `U(1,5)` at each endpoint |
| Parcel weight | Not applicable | Uniform integer in `{1,2,3}` |
| Revenue | `(1+direct_cost) * U(1.15,2.5)` | `(1+direct_cost) * U(1.15,2.5)` |

Every taxi has parcel capacity `5` and starts at time `0`. Revenue uses direct travel cost only; the objective later subtracts the actual cost of the complete taxi route. The generator does not create routes or precompute waiting. When evaluating a route, service at a windowed stop begins at `max(arrival, earliest)` and must have `arrival <= latest`. That wait delays the next stop. Passenger drop-off has no window and begins service immediately upon arrival. A taxi may finish at its last stop; no return trip or route-end horizon is imposed.

The paper used San Francisco traces and a road network, which are unavailable here. The square coordinates, independently perturbed directed matrices, parcel windows, service durations, capacities, weights, and revenues are **our generation assumptions**, not official contest data. Generation does not guarantee that any particular request is feasible or profitable. These cases are useful for solver development, but should not be described as Nguyen's published instances or as the contest's hidden distribution.

## Internal file layout

This layout is **our development format**, not a claimed SOICT judge format. The PDF specifies the data of a mathematical instance but does not specify file syntax, ordering, numeric types, or an output protocol. In particular, `SOICT2026 1` is our format marker.

```text
SOICT2026 1
K N M
K lines: parcel_capacity
N lines: revenue pickup_earliest pickup_latest service_duration
M lines: revenue weight pickup_earliest pickup_latest delivery_earliest delivery_latest service_duration
V lines of V travel-time values
V lines of V travel-cost values
```

Here `V = K + 2N + 2M`. Zero-based node IDs are ordered as taxi origins, passenger pickups, passenger drop-offs, parcel pickups, and parcel drop-offs. A point is represented by its node ID and its row/column in each matrix; no coordinates are required by the PDF. Thus the taxi-origin points and request endpoints are present even though no coordinate lines appear in the file.

| Contest input | Internal representation |
| --- | --- |
| Taxi count, initial point `O_k`, parcel capacity `Q_k` | `K`; origin node ID `k`; taxi capacity line `k` |
| Passenger endpoints `P_i`, `D_i`, revenue, pickup window, service duration | Implicit pickup/drop-off node IDs; one passenger line with the remaining fields |
| Parcel endpoints `P_j`, `D_j`, revenue, weight, pickup/delivery windows, service duration | Implicit pickup/drop-off node IDs; one parcel line with the remaining fields |
| Ordered-pair travel time `t(u,v)` and cost `c(u,v)` | Two separate `V` by `V` matrices, including taxi origins |

All taxis are available at time zero, so no per-taxi start time is stored. Routes, arrivals, waiting, loads, and objective value are solution data or derived values, not instance fields. `read_instance` validates counts, windows, nonnegative inputs, and matrix dimensions. `Solution::routes[k]` holds service-node IDs after taxi `k`'s origin; `evaluate` checks feasibility and recomputes benefit. Empty routes are valid.

When an official dataset and submission format become available, they will need their own reader/writer (or an explicit converter). The current reader accepts only this internal format.
