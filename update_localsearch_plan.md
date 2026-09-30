# Kế hoạch nâng cấp Local Search & ALNS — SoICT2026 (v4)

> **Revision history**: v1 → v2 (sửa 4 vấn đề lớn) → v3 (sửa 6 lỗi kỹ thuật + thứ tự) → v4 (sửa 5 lỗi code, chuẩn hóa Regret, thiết kế lại WorstRemoval/VND/Reheating, hoãn LS Gating, cải thiện quy trình kiểm chứng).

---

## Mục lục

1. [Bảng tổng hợp](#1-bảng-tổng-hợp)
2. [Phase 0 — Nền đo lường](#2-phase-0)
3. [Phase 1 — Merge patch Best-First + xác nhận tất định](#3-phase-1)
4. [Phase 2 — Cooling theo thời gian + Regret](#4-phase-2)
5. [Phase 3 — VND vòng + Randomized WorstRemoval + Tune nhẹ](#5-phase-3)
6. [Phase 4 — Replace](#6-phase-4)
7. [Phase 5 — Swap + Cache cross-route](#7-phase-5)
8. [Phase 6 — Reheating + Route Removal + Elite Pool](#8-phase-6)
9. [Phase 7 — Tune đầy đủ](#9-phase-7)
10. [Loại bỏ / Hoãn](#10-loại-bỏ)
11. [Quy trình đo lường và thống kê](#11-quy-trình-đo)
12. [Errata v1–v4](#12-errata)

---

## 1. Bảng tổng hợp

| # | Nâng cấp | Loại | Độ khó | Tác động | Phase |
|---|----------|------|--------|----------|-------|
| ✅ 0a | Benchmark script nhiều seed | Đo lường | 🟢 | Nền tảng | 0 |
| ✅ 0b | Validator debug per-move (hook via `config.check`) | Đo lường | 🟢 | Nền tảng | 0 |
| ✅ 0c | Profiling counters (trace CSV: seconds, progress) | Đo lường | 🟡 | Nền tảng | 0 |
| ✅ 0d | Chế độ `--iterations N` (tất định) + biến `progress` | Hạ tầng | 🟢 | Nền tảng | 0 |
| ✅ 1a | Merge Best-First PassengerRelocate | Tốc độ | 🟢 | Phụ thuộc 2a | 1 |
| ✅ 1b | Merge Best-First ParcelRelocate | Tốc độ | 🟢 | Phụ thuộc 2a | 1 |
| ✅ 1c | Assert đúng vị trí (Parcel: `pair.objective` vs `*gain`) | Debug | 🟢 | Nền tảng | 1 |
| ✅ 2a | Cooling theo thời gian (biến `progress`) | Chất lượng | 🟢 | **Rất cao** | 2 |
| ✅ 2b | Bật RegretInsertion (3 biến thể V0/V1/V2) | Chất lượng | 🟢 | Cao (cần đo) | 2 |
| ✅ 3a | VND vòng (sửa: `no_improve = 1`, cap `4n`) | Chất lượng | 🟢 | Trung bình-Cao | 3 |
| ✅ 3b | Randomized WorstRemoval (Ropke–Pisinger, `nth_element`) | Chất lượng | 🟢 | Trung bình-Cao | 3 |
| 3c | Tune nhẹ T₀ và ls_interval | Chất lượng | 🟡 | Cao | 3 |
| ✅ 4a | Neighborhood Replace | Chất lượng | 🟡 | Trung bình (cần đo) | 4 |
| 5a | Swap inter-route (SWAP* style) | Chất lượng | 🔴 | Trung bình-Cao | 5 |
| 5b | Cache cross-route: lưu min_delta | Tốc độ→CL | 🔴 | Phụ thuộc profile | 5 |
| 6a | Reheating (neo lịch trình, reset stagnation) | Chất lượng | 🟡 | Trung bình | 6 |
| 6b | Route Removal ngẫu nhiên | Chất lượng | 🟢 | Thấp-Trung bình | 6 |
| 6c | Elite Pool | Chất lượng | 🟡 | Trung bình | 6 |
| 7 | Tune đầy đủ (irace, tập tune/test riêng) | Chất lượng | 🟡 | Cao | 7 |

> LS Gating (v3 Phase 3c) đã **hoãn** — xem [mục 10](#10-loại-bỏ).

---

## 2. Phase 0 — Nền đo lường ✅ DONE

**Implementation notes:**
- [`scripts/benchmark.py`](file:///d:/Work/SLSCM-LaB/contest/src2/scripts/benchmark.py) — multi-seed runner, parallel jobs, gap vs BKS, output raw + summary CSV. Hỗ trợ `--iterations N` mode.
- [`ALNS.cpp`](file:///d:/Work/SLSCM-LaB/contest/src2/src/search/alns/ALNS.cpp) — vòng lặp dùng biến `progress ∈ [0,1]`. Trace CSV thêm cột `seconds` và `progress`.
- [`Config.hpp`](file:///d:/Work/SLSCM-LaB/contest/src2/include/search/alns/Config.hpp) — thêm `std::optional<std::size_t> max_iterations`.
- [`main.cpp`](file:///d:/Work/SLSCM-LaB/contest/src2/src/main.cpp) — thêm `--iterations N` CLI flag.
- **Known TODO**: `config.cooling` bị silently ignored trong ALNS.cpp mới. Cần thêm warning hoặc xóa khi infrastructure ổn định.
- **0b per-move validator** đang thông qua `config.check` (gọi `SearchState::check()` sau mỗi candidate). Inline assert trong từng operator sẽ được thêm khi implement Phase 1.

### 0a. Benchmark script

- 10–20 seed/instance, báo `mean ± std` gap so với BKS.
- Đơn vị độc lập là **instance** (không phải seed). So sánh cặp theo instance bằng **Wilcoxon signed-rank test**.
- BKS từ `data/synthetic/best-known/best-known.csv`. MIP optimal chỉ hữu ích nếu ALNS chưa chạm optimum trên `r10_*`.
- Ghim mỗi tiến trình vào **một core vật lý** (`taskset -c N` / `start /affinity`). Không chạy quá số core vật lý.
- Sàng lọc nhanh ở 15–20s trên tập con (5 instance × 5 seed) trước khi chạy đầy đủ.

### 0b. Validator debug per-move

`SearchState::check()` (dòng 185–190) đã so `benefit_` với `solution().evaluate()`. Bọc nó thành validator per-move:

```cpp
// Trong mỗi operator, bọc mỗi mutation:
#ifndef NDEBUG
    const double before = state.benefit();
#endif
    // ... apply move (remove + insert / relocate) ...
#ifndef NDEBUG
    const double actual_delta = state.benefit() - before;
    assert(std::abs(predicted_gain - actual_delta)
           < 1e-6 * std::max(1.0, std::abs(actual_delta)));
    state.check();
#endif
```

Đây kiểm tra `calculate_delta_fast` có đúng hay không — **bắt buộc** trước mọi operator mới.

### 0c. Profiling counters

Debug build, đếm:
- Số lần `FastRouteCheck::feasible()` per `best_on_taxi`, cache hit rate `MoveCache`.
- Thời gian mỗi LS operator: Remove, Add, PassengerRelocate, ParcelRelocate.
- Phân phối số vòng VND, tỉ lệ LS cải thiện candidate.

### 0d. Chế độ `--iterations N` + biến `progress ∈ [0,1]`

Thêm option `--iterations N` vào CLI. Cooling dùng **biến `progress` thống nhất** cho cả hai chế độ:

```cpp
const double progress = config.max_iterations
    ? static_cast<double>(iterations) / *config.max_iterations
    : elapsed / config.time_limit;
temperature = T0 * std::pow(Tmin / T0, std::min(progress, 1.0));
```

Vòng lặp dừng khi `progress >= 1.0`:
```cpp
while (progress < 1.0) { ... }
```

Mục đích: cùng seed + cùng N → cùng quỹ đạo → xác nhận patch không đổi objective.

---

## 3. Phase 1 — Merge patch Best-First + xác nhận tất định ✅ DONE

**Implementation notes:**
- [`PassengerRelocate.cpp`](file:///d:/Work/SLSCM-LaB/contest/src2/src/search/alns/operators/LocalSearch/PassengerRelocate.cpp): `best_on_taxi` được thay bằng 3 bước: (1) thu thập objective O(1) cho mọi vị trí, (2) sort giảm dần, (3) duyệt best-first gọi `gain()` theo thứ tự. `thread_local` vector tránh malloc mỗi cache miss.
- [`ParcelRelocate.cpp`](file:///d:/Work/SLSCM-LaB/contest/src2/src/search/alns/operators/LocalSearch/ParcelRelocate.cpp): Tương tự nhưng cho cặp (a,b). `thread_local ParcelScratch` tái sử dụng 5 vector. Tái sử dụng prefix FastRouteCheck khi cùng pickup pos.
- **Assert 1c**: trong `ParcelRelocate::best_on_taxi`, sau `gain_from_prefix`, assert `|*gain - pair.objective| < 1e-6`. Chỉ chạy trong debug build (không có `NDEBUG`).
- **Lưu ý**: `PassengerRelocate::gain()` vẫn kết thúc bằng `return calculate_delta_fast()` — assert Passenger không được viết (so hàm với chính nó).
- **Bước xác nhận cần build**: Chạy `--iterations 50000 --check` cồng sàng baseline vs patched. TODO sau khi user approve build.

**Phải làm TRƯỚC cooling theo thời gian.** Cooling theo thời gian làm mất tính tất định.

### 1a & 1b. Merge patch

Copy 2 file từ `idea_update_localsearch/`:
- `PassengerRelocate.cpp`
- `ParcelRelocate.cpp`

**KHÔNG merge** `LocalSearch.cpp` (bản `kMaxPasses`). VND vòng viết riêng ở Phase 3.

### 1c. Xác nhận objective trùng

**Bước 1**: Chạy baseline **2 lần cùng seed** trước. Nếu khác nhau (ví dụ do non-determinism ẩn) thì mọi so sánh với patch đều vô nghĩa. `active_routes_` dùng `std::map` (ordered) — nên deterministic, nhưng cần xác nhận.

**Bước 2**: So sánh baseline vs patch ở chế độ tất định:
```bash
# Baseline
./SoICT2026 solve-alns --instance X --seed 1 --iterations 50000 --out old.txt
# Patch
./SoICT2026_patched solve-alns --instance X --seed 1 --iterations 50000 --out new.txt
```

**Bước 3**: So **vết (iteration, benefit) mỗi 1000 iterations**, không chỉ objective cuối. Tìm điểm rẽ nhánh đầu tiên. Tie-break 1e-12 do floating-point có thể gây rẽ nhánh muộn — chấp nhận nếu benefit cuối khác nhau < 1e-9.

### 1d. Assert đúng vị trí

**Passenger**: Assert `gain == calculate_delta_fast` vô nghĩa vì `gain()` return `calculate_delta_fast()` — so hàm với chính nó. Kiểm tra thực sự là validator per-move ở 0b.

**Parcel** (trong patch `ParcelRelocate::best_on_taxi`): Assert `pair.objective` vs `*gain` — hai đường tính khác nhau:
- `pair.objective = savings - delta` (delta từ mảng `pickup_cost[]`/`delivery_cost[]`)
- `*gain` từ `gain_from_prefix()` → `calculate_delta_fast()` (cũng `separated_insertion_cost` nhưng qua `RouteView` logic khác)

```cpp
// Trong patch ParcelRelocate::best_on_taxi, khi gain có giá trị:
#ifndef NDEBUG
    assert(std::abs(*gain - pair.objective)
           < 1e-6 * std::max(1.0, std::abs(*gain)));
#endif
```

Cover: `a == b` (adjacent), cùng route (ánh xạ `original_position`).

---

## 4. Phase 2 — Cooling theo thời gian + Regret

> **Baseline mới**: Mọi so sánh sau Phase 2 dùng cooling theo thời gian cho **cả hai bên**.

### 2a. Cooling theo thời gian

Dùng biến `progress` từ 0d. Khi ở chế độ thời gian:
```cpp
const double progress = elapsed / config.time_limit;
temperature = T0 * std::pow(Tmin / T0, std::min(progress, 1.0));
```

Vòng lặp dừng khi `elapsed >= config.time_limit` (hoặc `progress >= 1.0`).

**Scale T₀ theo instance**: Dùng `std::optional<double>` trong Config:
```cpp
// Config.hpp:
std::optional<double> temperature; // nullopt = tự tính từ instance
```

**Hiệu chỉnh T₀**: Chạy N≈200 iteration warm-up, thu thập |Δ| khi Δ < 0. Đặt T₀ sao cho P(accept) ≈ 30–50% ban đầu:
```cpp
T0 = -mean_negative_delta / std::log(0.4);
Tmin = T0 * 1e-4; // giữ tỉ lệ T₀/Tmin cố định
```

### 2b. Bật RegretInsertion (3 biến thể)

Đọc code thật (`RegretInsertion.cpp` dòng 18–20, `consider()` trong `InsertionSearch.cpp` dòng 51–56):

**`second_gain` CÓ THỂ âm**: `consider()` cập nhật `second_gain = std::max(second_gain, choice.gain)` cho mọi insertion khả thi, kể cả gain âm. `scan_route` truyền `data.revenue - delta` vào `consider()` mà không lọc dấu.

**Ba biến thể thực sự**:

| Biến thể | Công thức regret | Khi chỉ 1 option (second = -∞) | Khi second < 0 |
|----------|-----------------|-------------------------------|----------------|
| **V0** (hiện tại) | `best - second` nếu second finite; `best` nếu không | regret = best (trùng hợp = `best - 0`) | regret = best - second (lớn hơn best) |
| **V1** | `best - max(second, 0)` | regret = best | regret = best (bỏ qua second âm) |
| **V2** | `+∞` khi 1 option; `best - second` khi 2+ options | regret = +∞ (ưu tiên tuyệt đối) | regret = best - second |

> V2 cần tie-break bằng `best->gain`. Dòng 21–22 **đã có**: `regret == best_regret && options.best->gain > choice->gain` — phù hợp.

> V0 **KHÔNG** tương đương `best - max(second, 0)`: khi second hữu hạn nhưng âm, V0 cho regret > best, còn V1 cho regret = best. Errata v3 "ngầm coi không phục vụ = gain 0" **chỉ đúng cho trường hợp 1-option**, gây hiểu nhầm khi khái quát.

Thêm vào `ALNS.cpp`:
```cpp
#include "search/alns/operators/Repair/RegretInsertion.hpp"
// ...
repairs.push_back(std::make_unique<GreedyInsertion>());
repairs.push_back(std::make_unique<RegretInsertion>());
```

**Đo**: Greedy-only vs Greedy+Regret(V0) vs Greedy+Regret(V1) vs Greedy+Regret(V2). 10+ seeds, cooling theo thời gian.

---

## 5. Phase 3 — VND vòng + Randomized WorstRemoval + Tune nhẹ

### 3a. VND vòng

```cpp
bool LocalSearch::search(SearchState& state) const {
    if (moves_.empty()) return false;
    bool any_change = false;
    std::size_t no_improve_count = 0;
    std::size_t i = 0;
    const std::size_t max_calls = 4 * moves_.size(); // dynamic cap
    std::size_t total_calls = 0;
    while (no_improve_count < moves_.size() && total_calls < max_calls) {
        const bool changed = moves_[i]->improve(state);
        if (changed) {
            any_change = true;
            no_improve_count = 1; // move vừa chạy đã hội tụ nội bộ
        } else {
            ++no_improve_count;
        }
        i = (i + 1) % moves_.size();
        ++total_calls;
    }
    return any_change;
}
```

**Giải thích `no_improve_count = 1`**: `improve()` tự lặp đến hội tụ. Sau khi move j cải thiện và hội tụ, chạy lại j sẽ chỉ là cache-lookup thừa. Đặt `= 1` (đếm chính j là đã hội tụ) → chỉ cần n−1 move còn lại liên tiếp không đổi là dừng.

**Cap `4 * moves_.size()`**: Giới hạn an toàn khi thời gian bị chặn. Dynamic theo số operator hiện tại (tăng khi thêm Replace, Swap).

**Thứ tự moves**: Từ rẻ đến đắt: `Remove → Add → PassengerRelocate → ParcelRelocate`.

**Ghi log** phân phối `total_calls` trong debug build.

### 3b. Randomized WorstRemoval (Ropke–Pisinger)

Code hiệu quả (không sort lại toàn bộ mỗi lần xóa):

```cpp
std::vector<std::size_t> WorstRemoval::apply(SearchState& state, std::size_t count,
                                              std::mt19937_64& rng) const {
    constexpr double p = 3.0;
    // Bước 1: tính danh sách (gain, request) một lần
    std::vector<std::pair<double, std::size_t>> candidates;
    for (std::size_t r = 0; r < state.request_count(); ++r) {
        const auto gain = state.removal_gain(r);
        if (gain) candidates.emplace_back(*gain, r);
    }

    std::vector<std::size_t> removed;
    removed.reserve(count);
    std::uniform_real_distribution<double> uni(0, 1);

    while (removed.size() < count && !candidates.empty()) {
        // Bước 2: chọn hạng ⌊y^p · n⌋ bằng nth_element (O(n) thay vì O(n log n))
        const auto idx = std::min(
            static_cast<std::size_t>(std::pow(uni(rng), p) * candidates.size()),
            candidates.size() - 1);
        std::nth_element(candidates.begin(), candidates.begin() + idx,
                         candidates.end(), std::greater<>());
        const auto chosen = candidates[idx].second;
        const auto taxi = state.location(chosen).taxi;
        if (!state.remove(chosen)) {
            candidates.erase(candidates.begin() + idx);
            continue;
        }
        removed.push_back(chosen);
        candidates.erase(candidates.begin() + idx);

        // Bước 3: chỉ cập nhật gain cho request cùng route
        for (auto& [gain, r] : candidates) {
            if (state.location(r).taxi == taxi)
                gain = state.removal_gain(r).value_or(-1e18);
        }
        std::erase_if(candidates, [](const auto& c) { return c.first <= -1e17; });
    }
    return removed;
}
```

Dùng `rng` của solver (tham số đã có), không tạo RNG mới.

### 3c. Tune nhẹ T₀ và ls_interval

Chỉ tune 2 tham số:
- `T₀`: Thử 3–5 giá trị quanh mức hiệu chỉnh từ 2a.
- `ls_interval`: Thử 1, 5, 10, 20.

**Không tune đầy đủ** — bộ operator chưa đóng băng, sẽ phải tune lại ở Phase 7.

---

## 6. Phase 4 — Replace ✅ DONE

**Implementation notes:**
- Tạo `ReplaceRequest.cpp` duyệt mọi unserved request `r_in` và served request `r_out`. Tính exact bounds và check tính khả thi qua `RouteCandidate`.
- Sử dụng chính hạ tầng của operator Relocate (như `single_insertion_cost`, `adjacent_insertion_cost` và `RouteCandidate` để tránh lặp code).
- Đã được register trong `ALNS.cpp` (chạy cuối trong `LocalSearch::search()` sau khi Remove, Add, Relocate đã fail tìm improvement, giúp cho Replace chỉ kích hoạt khi vùng lân cận cơ bản đạt local optimum).
- File source đã được build vào `CMakeLists.txt`.

### 4a. Neighborhood Replace

**Công thức đúng**:
```
net_gain = [cost_saved(r_out) - revenue(r_out)]   // removal gain
         + [revenue(r_in) - cost_insert(r_in)]     // insertion gain
```

**Phạm vi chèn**: Chỉ xét chèn `r_in` vào **chính route vừa giải phóng** `r_out`. Nếu `r_in` chèn được ở route khác với gain > 0 thì `AddRequest` đã xử lý; nếu gain ≤ 0 thì tổng cũng ≤ 0.

**Tái sử dụng hạ tầng**: `RouteView` (ánh xạ vị trí sau khi bỏ 2 node) và `add_source_range` có thể tách ra dùng chung. Replace chính là "relocate cùng route nhưng chèn request khác". Cần context mới (`ReplaceContext`) nhưng logic ánh xạ vị trí kế thừa từ Relocate.

**Lower bound**: Chi phí chèn dựa trên arc-cost → O(1). Pruning trước FastRouteCheck rất hiệu quả.

**Kỳ vọng tác động**: **Trung bình (cần đo)**. Destroy/Repair đã khám phá thay thế 1-1 ngẫu nhiên. Replace hội tụ nhanh hơn nhưng chưa chắc nâng ceiling cuối.

---

## 7. Phase 5 — Swap + Cache cross-route

### 5a. Swap inter-route (SWAP* — Vidal 2022)

- Với mỗi (route, request): giữ top-K vị trí chèn tốt nhất.
- Ghép swap O(1): `gain_swap ≈ insert(A,route_B) + insert(B,route_A) - cost(A) - cost(B)`.
- Top-K làm **cận trên**, chỉ gọi FastRouteCheck khi cận trên dương.
- Time window làm top-K không đủ đảm bảo khả thi → dùng làm bộ lọc, không thay thế feasibility check.
- O(N²) cặp → cần **neighbor list** + chạy mỗi M lần LS.

### 5b. Cache cross-route: lưu min_delta

**Hiện trạng** (`MoveCache.hpp`): Cross-route positive moves đã có `adjusted.gain += source_saving - entry.source_saving` — **hoạt động đúng**. Negative moves bị invalidate khi `source_saving` tăng.

**Pool an toàn**: `pool.add()` lọc `choice->gain > 1e-9`. `MoveCache.get()` trả adjusted gain. Nếu adjusted ≤ 1e-9, pool loại bỏ → **không vòng vô hạn**.

**Cải thiện**: Lưu `min_insertion_cost` (feasible) thay vì gain → `gain = saving - min_cost`. Khi saving đổi, tính lại gain mà không rescan. Chi phí: lần điền cache tốn hơn. **Chỉ đáng nếu profiling (0c) cho thấy rescan do saving tăng là lớn.**

---

## 8. Phase 6 — Reheating + Route Removal + Elite Pool

### 6a. Reheating (neo lịch trình, reset stagnation)

**Lỗi v3 đã sửa**:
- `last_improvement_time` phải reset khi reheat (nếu không → reheat liên tục).
- Ngưỡng = phần trăm cố định tổng thời gian (không phải `(limit - elapsed) * 0.1`).
- Neo lại lịch trình để T(t) không ghi đè temperature mới.

```cpp
// Trạng thái SA:
double anchor_temp = T0;
double anchor_time = 0;
double last_improvement_time = 0;

// Trong vòng lặp:
const double local_progress = (elapsed - anchor_time) / (config.time_limit - anchor_time);
temperature = anchor_temp * std::pow(Tmin / anchor_temp, std::min(local_progress, 1.0));

// Cập nhật stagnation:
if (current.benefit() > best.benefit() + 1e-9) {
    best = current;
    last_improvement_time = elapsed;
}

// Reheat:
if (elapsed - last_improvement_time > 0.05 * config.time_limit
    && elapsed < 0.8 * config.time_limit) {
    anchor_temp = 0.2 * T0;
    anchor_time = elapsed;
    last_improvement_time = elapsed;  // ← BẮT BUỘC: reset tránh reheat liên tục
    current = best;
}
```

### 6b. Route Removal ngẫu nhiên

Weighted sampling nghịch đảo số request. Tác động: Thấp–Trung bình.

### 6c. Elite Pool

Làm sau khi có số liệu stagnation.

---

## 9. Phase 7 — Tune đầy đủ

Sau khi bộ operator **đóng băng**:

1. **Tách data**: `data/synthetic/` → 60% tune + 40% test. Không tune trên tập test.
2. **Tune chung** (irace hoặc grid): T₀, ls_interval, cooling, Balance decay/scores. Các tham số **tương tác mạnh** (VND × ls_interval × cooling) → tune chung, không từng cái.
3. **Đánh giá cuối** trên tập test, Wilcoxon signed-rank vs baseline.

> Tune nhẹ ở Phase 3 chỉ cho T₀ và ls_interval (quyết định ngân sách). Tune đầy đủ ở đây.

---

## 10. Loại bỏ / Hoãn

| Mục | Trạng thái | Lý do |
|-----|-----------|-------|
| Or-opt | **Loại bỏ** | PassengerRelocate phủ passenger, ParcelRelocate thử mọi (a,b) cùng route |
| Time-based Removal | **Loại bỏ** | RelatedRemoval (Shaw) đã dùng chênh earliest time window |
| Intra-route DP | **Hoãn** | Xem xét lại nếu profiling cho thấy intra-route bottleneck trên route dài |
| **LS Gating** | **Hoãn** | LS có thể "cứu" candidate tệ thành tốt; gating trước LS bỏ lỡ cơ hội này. Chạy LS trên `current` sau accept cũng thay đổi quyết định accept (phải dời kiểm tra new-best). Chỉ xem xét **sau profiling (0c)** cho thấy LS chiếm phần lớn tổng thời gian, và nếu làm thì `tolerance` phải tune cùng T₀ + ls_interval ở Phase 7 |

---

## 11. Quy trình đo lường và thống kê

### Ngân sách

21 instance × 10 seed × 60s ≈ 3.5h/biến thể/core. Sàng lọc 15–20s trên tập con trước.

### Thống kê

- **Đơn vị**: Instance. Gap tương đối: `gap_i = (BKS_i - mean_ALNS_i) / BKS_i`.
- **So sánh**: Wilcoxon signed-rank trên gap per instance. P < 0.05 = có ý nghĩa.
- **Ý nghĩa thực tế**: Nếu `std/√10 ≈ 0.3%` thì chênh 0.5% chưa tách được nhiễu → cần 20 seed hoặc nhiều instance.

### Quy tắc đánh giá

Mỗi thay đổi **xét riêng**, giữ hoặc bỏ dựa trên giá trị kỳ vọng / chi phí. **KHÔNG** áp dụng "Phase N nhỏ → bỏ N+1". Chỉ 2 phụ thuộc thật:
- Giá trị tăng tốc (1a/1b) phụ thuộc cooling theo thời gian (2a).
- Mọi operator phụ thuộc tune cuối (7).

---

## 12. Errata v1–v4

| Mục | Lỗi | Sửa (version) |
|-----|-----|---------------|
| Thứ tự Phase cooling/patch | Cooling trước → mất tất định | Patch trước, cooling sau (v3) |
| Assert Passenger | `gain == delta` so hàm với chính nó | Chỉ assert Parcel + per-move validator (v3) |
| Assert Parcel (v3) | Dùng tên biến code cũ (`a`, `b`, `adjacent_cost`) | Dùng `pair.objective` vs `*gain` từ patch (v4) |
| VND `no_improve_count = 0` | Move j chạy lại thừa | `= 1` (v3) |
| VND `kMaxCalls = 20` | Cố định, không scale theo số operator | `4 * moves_.size()` (v4) |
| Reheating gán T trực tiếp | Bị T(t) ghi đè | Neo (anchor_temp, anchor_time) (v3) |
| Reheating không reset stagnation | `last_improvement_time` không reset → reheat liên tục | `last_improvement_time = elapsed` khi reheat (v4) |
| Reheating ngưỡng ngược | `(limit-elapsed)*0.1` dễ hơn gần cuối | `0.05 * limit` cố định (v4) |
| Regret "ngầm coi gain=0" | Chỉ đúng 1-option case, sai tổng quát | 3 biến thể V0/V1/V2, mô tả chính xác (v4) |
| Cooling scale T₀ không scale Tmin | Tỉ lệ T₀/Tmin bị đổi | `Tmin = T₀ × 1e-4` cố định (v3) |
| Cooling `== Config{}.temperature` | Sai khi user truyền đúng default | `std::optional<double>` (v3) |
| Replace công thức | Đếm doanh thu hai lần | Sửa đúng (v3) |
| Quy tắc dừng phase | "< 0.5% → bỏ N+1" | Xét riêng từng thay đổi (v3) |
| WorstRemoval code v3 | O(q·R log R), mâu thuẫn chú thích | `nth_element` + cập nhật cùng route (v4) |
| LS Gating | Bỏ lỡ LS "cứu" candidate; tolerance chưa định nghĩa | Hoãn sau profiling (v4) |
| `--iterations` cooling | Dùng cooling 60000 cho N=50000 | Biến `progress ∈ [0,1]` thống nhất (v4) |

---

## Thứ tự triển khai (v4)

```
Phase 0 → Benchmark script, validator per-move, profiling, --iterations mode (progress)
Phase 1 → Merge Best-First patch, xác nhận deterministic (baseline 2 lần + vết),
          assert Parcel pair.objective vs *gain
Phase 2 → Cooling theo thời gian (đặt lại baseline), Regret 3 biến thể
Phase 3 → VND vòng (no_improve=1, cap 4n), Randomized WorstRemoval, tune nhẹ T₀/ls_interval
Phase 4 → Replace
Phase 5 → Swap SWAP* (nếu profiling cho thấy cần)
Phase 6 → Reheating (neo lịch trình + reset stagnation), Route Removal, Elite Pool
Phase 7 → Tune đầy đủ trên tập tune, đánh giá cuối trên tập test
```
