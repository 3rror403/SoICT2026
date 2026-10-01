# plan_update.md

# SoICT2026 Solver Update Plan

## 1. Mục tiêu

Tài liệu này tổng hợp kế hoạch cập nhật solver cho bài toán **Integrated Passenger and Parcel Transportation – SOICT 2026**, dựa trên kiến trúc hiện tại gồm:

- `soict_core`: domain logic, route representation, feasibility;
- `RouteCache` / `Segment` / `FastRouteCheck`: đánh giá nhanh route;
- `soict_alns`: ALNS + SA + local search;
- `soict_mip`: formulation / exact components;
- các operator hiện có:
  - Destroy: Random, Worst, Related, Route Removal;
  - Repair: Greedy, Regret;
  - LS: Relocate, Replace, SWAP*.

Bản code mới đã có **SA theo thời gian và reheating**; vì vậy việc chuyển từ cooling theo iteration sang time-based SA **không còn là hạng mục cần cập nhật**. File `architecture.md` hiện tại vẫn mô tả pseudo-code cũ theo nhiệt độ và cần được đồng bộ lại sau.

---

# 2. Nguyên tắc ưu tiên

Ưu tiên cải tiến theo ba tiêu chí:

1. **Đúng trước, nhanh sau**
   - Các tối ưu `O(1)` và pruning đều dựa vào `Segment`.
   - Nếu `Segment::merge()` sai dù hiếm, toàn bộ solver có thể trả nghiệm sai hoặc bỏ mất neighborhood tốt.

2. **Ưu tiên cải thiện quality / second**
   - Không thêm operator chỉ vì neighborhood lớn.
   - Đo:
     \[
     \frac{\Delta \text{BestBenefit}}{\text{CPU second}}
     \]
     cho từng operator / LS tier.

3. **Khai thác cấu trúc riêng của bài toán**
   - Passenger là request gần như atomic:
     \[
     P_i^n \rightarrow D_i^n
     \]
     và không được chèn node ở giữa.
   - Parcel có hai vị trí pickup/delivery, precedence, capacity và time windows.
   - Request có thể bị từ chối.
   - Taxi có chi phí xuất phát ngầm:
     \[
     c(O_k,v_1).
     \]

---

# 3. Thứ tự triển khai

## Priority 0 — Property/Fuzz Test cho `Segment`

### Mục tiêu

Xác minh tuyệt đối tính đúng của:

- `Segment::merge(A, B)`;
- tính kết hợp:
  \[
  (A \oplus B)\oplus C = A\oplus(B\oplus C);
  \]
- `RouteCache`;
- `FastRouteCheck`;
- các statistic:
  - duration;
  - cost;
  - ready time;
  - latest input;
  - load delta;
  - min/max load;
  - time feasibility.

### Test 1 — Merge vs direct simulation

Sinh route ngẫu nhiên:

```text
R = [v0, v1, ..., vn]
```

Chọn ngẫu nhiên đoạn:

```text
A = R[l:m]
B = R[m:r]
```

So sánh:

```text
merge(segment(A), segment(B))
```

với:

```text
simulate_directly(A + B)
```

Kiểm tra mọi trường.

### Test 2 — Associativity

Sinh ngẫu nhiên ba đoạn liên tiếp:

```text
A | B | C
```

So sánh:

```text
merge(merge(A, B), C)
```

và:

```text
merge(A, merge(B, C))
```

### Test 3 — Fast insertion vs full recomputation

Với một request ngẫu nhiên:

- passenger insertion;
- parcel insertion;
- relocate;
- replace;

so sánh:

```text
FastRouteCheck
```

với:

```text
full route simulation
```

### Test 4 — Edge cases

Bao gồm:

- route rỗng;
- insertion đầu route;
- insertion cuối route;
- passenger adjacent pickup/drop-off;
- parcel pickup và delivery cùng arc;
- capacity đúng bằng `Q`;
- arrival đúng bằng `latest`;
- arrival trước `earliest`;
- taxi capacities khác nhau;
- asymmetric cost/time.

### Acceptance criterion

Chạy ít nhất:

```text
10^6 random merge/evaluation cases
```

không có mismatch.

---

# 4. Priority 1 — Worst Removal theo Marginal Benefit

## Vấn đề hiện tại

Pseudo-code `WorstRemoval` dùng:

\[
cost\_savings(r)
=
C(R)-C(R\setminus r).
\]

Điều này chỉ đo travel cost, trong khi objective là:

\[
Benefit
=
\sum Revenue
-
TravelCost.
\]

## Score mới

Removal gain thật sự:

\[
G_{\text{remove}}(r)
=
C(R)-C(R\setminus r)-Revenue(r).
\]

Diễn giải:

- nếu
  \[
  G_{\text{remove}}(r)>0,
  \]
  bỏ request đó ngay lập tức làm tăng objective;
- request có `G_remove` càng lớn càng là ứng viên tốt để remove.

## Giữ hai operator

Không thay thế hoàn toàn operator cũ.

### Operator A

```text
WorstTravelRemoval
```

Score:

\[
C(R)-C(R\setminus r)
\]

Mục đích: phá các request gây routing cost lớn.

### Operator B

```text
WorstMarginalBenefitRemoval
```

Score:

\[
C(R)-C(R\setminus r)-Revenue(r)
\]

Mục đích: loại các request economically harmful.

### Randomization

Giữ randomized Ropke–Pisinger:

```cpp
idx = floor(pow(U(0,1), p) * size)
```

với:

```text
p ≈ 3
```

và cho ALNS học weight hai operator độc lập.

---

# 5. Priority 2 — Exact Parcel Insertion bằng Cost-Ordered Heap Enumeration

## 5.1. Mục tiêu

Hiện parcel insertion naïve có thể cần:

\[
O(L^2)
\]

cặp `(pickup_pos, delivery_pos)` trên một route.

Mục tiêu mới:

- vẫn **exact**;
- không dùng top-B approximation;
- enumerate candidate theo **travel-cost increase tăng dần**;
- feasibility được kiểm tra lazy;
- dừng ngay khi đã chứng minh không còn candidate tốt hơn.

---

## 5.2. Delta cost khi pickup và delivery nằm trên hai arc khác nhau

Giả sử pickup chèn sau arc:

\[
(A_i,B_i)
\]

và delivery chèn sau arc:

\[
(A_j,B_j), \qquad j>i.
\]

Pickup delta:

\[
\Delta_p(i)
=
c(A_i,P)+c(P,B_i)-c(A_i,B_i).
\]

Delivery delta:

\[
\Delta_d(j)
=
c(A_j,D)+c(D,B_j)-c(A_j,B_j).
\]

Khi hai insertion nằm trên hai arc khác nhau:

\[
\boxed{
\Delta(i,j)
=
\Delta_p(i)+\Delta_d(j)
}
\]

chính xác.

Không có interaction cost bổ sung.

---

## 5.3. Trường hợp pickup và delivery cùng một arc

Nếu parcel được chèn liên tiếp:

```text
A -> P -> D -> B
```

thì:

\[
\Delta_{\text{adj}}(i)
=
c(A_i,P)
+c(P,D)
+c(D,B_i)
-c(A_i,B_i).
\]

Case này phải xử lý riêng.

---

## 5.4. Constraint thứ tự

Với hai arc khác nhau, cần:

\[
j>i.
\]

Do đó với pickup position `i`, delivery candidate thuộc suffix:

\[
\{i+1,\ldots,L\}.
\]

Không thể chỉ lấy Cartesian product của hai danh sách đã sort toàn cục.

---

## 5.5. Data structure

Cần hỗ trợ:

> Với mỗi `i`, lấy phần tử nhỏ thứ `k` của
> \[
> \{\Delta_d(j)\mid j>i\}.
> \]

Khuyến nghị:

### Persistent segment tree trên rank của `Delta_d`

Preprocessing trên mỗi route/request:

\[
O(L\log L).
\]

Query:

```text
k-th smallest delta_d in suffix [i+1, L]
```

trong:

\[
O(\log L).
\]

Một lựa chọn đơn giản hơn để thử nghiệm trước:

- sort suffix candidates offline;
- hoặc dùng vector suffix nhỏ nếu route ngắn.

Persistent tree chỉ triển khai nếu profiling cho thấy đáng giá.

---

## 5.6. Heap enumeration

Với mỗi pickup position `i`:

1. lấy delivery nhỏ nhất trong suffix `i+1...L`;
2. push candidate vào min-heap:

```text
(delta_p[i] + delta_d[j], i, rank=1)
```

Sau đó:

```cpp
while (!heap.empty()) {
    Candidate x = heap.pop_min();

    if (x.cost_delta >= best_known_delta)
        break;

    if (FastRouteCheck::parcelInsertion(route, request, x.i, x.j)) {
        best_known_delta = x.cost_delta;
        best = x;

        // Vì heap đang ordered exact theo cost:
        // nếu chỉ cần best feasible insertion trên route,
        // có thể return ngay tại feasible candidate đầu tiên.
        return best;
    }

    int next_rank = x.rank + 1;
    int j2 = kthSmallestDeliveryInSuffix(x.i + 1, next_rank);

    if (j2 exists)
        heap.push({
            delta_p[x.i] + delta_d[j2],
            x.i,
            j2,
            next_rank
        });
}
```

Các candidate adjacency:

```text
A -> P -> D -> B
```

được push vào cùng heap.

---

## 5.7. Correctness

Vì:

\[
\Delta(i,j)
=
\Delta_p(i)+\Delta_d(j)
\]

là exact travel-cost delta, heap enumerate candidate theo đúng thứ tự tăng dần của objective contribution.

Do time/waiting không trực tiếp xuất hiện trong objective, feasibility chỉ là bộ lọc.

Vì vậy:

> Candidate feasible đầu tiên được pop là insertion có travel-cost increase nhỏ nhất.

Nếu đang xét benefit của request `r`:

\[
gain(r,i,j)
=
Revenue(r)-\Delta(i,j).
\]

Revenue là hằng số theo request, nên minimum `Delta` tương đương maximum gain.

---

## 5.8. Worst case

Worst case vẫn có thể enumerate:

\[
O(L^2)
\]

nếu hầu hết candidate infeasible.

Nhưng expected practical complexity có thể thấp hơn rất nhiều vì:

- time windows prune mạnh;
- capacity prune mạnh;
- delivery TW hẹp;
- best feasible candidate thường xuất hiện sớm;
- có thể dùng route-level lower bound để bỏ route.

---

## 5.9. Intra-route relocate

Khi relocate request đang nằm trong route:

> mọi delta phải được tính trên `route_without_request`.

Không dùng arc structure của route gốc vì removal đã thay đổi các arc lân cận.

---

# 6. Priority 3 — Clean-Cut 2-opt*

## 6.1. Ý tưởng

Hai route:

\[
R_a=A_1|A_2
\]

\[
R_b=B_1|B_2
\]

được cắt tại hai **clean cut**.

Tạo:

\[
R'_a=A_1|B_2
\]

\[
R'_b=B_1|A_2.
\]

Đây là 2-opt* trên hai route.

---

## 6.2. Định nghĩa clean cut

Boundary giữa hai node là clean nếu:

1. không cắt giữa pickup/drop-off passenger;
2. không có parcel đang onboard qua boundary;
3. không có request nào có một endpoint ở prefix và endpoint còn lại ở suffix.

Trong thực tế có thể kiểm tra nhanh bằng:

```text
load == 0
```

nếu route representation đảm bảo mọi parcel là nguồn duy nhất của carried load và passenger luôn contiguous.

Để an toàn, có thể duy trì thêm:

```text
open_request_count == 0
```

hoặc prefix/suffix request-balance signature trong debug build.

---

## 6.3. Vì sao clean cut quan trọng

Nếu cut sạch:

- mọi request nằm trọn trong prefix hoặc tail;
- precedence được giữ;
- same-taxi pickup/delivery được giữ sau khi tail chuyển sang taxi khác;
- không cần repair request pair.

---

## 6.4. Cost delta

Với cut sau node `a` của route A và sau node `b` của route B:

trước:

```text
a -> a_next
b -> b_next
```

sau:

```text
a -> b_next
b -> a_next
```

Travel-cost delta:

\[
\Delta C
=
c(a,b_{next})
+c(b,a_{next})
-c(a,a_{next})
-c(b,b_{next}).
\]

Với open route, nếu một cut nằm cuối route thì phải xử lý riêng vì không có outgoing arc.

---

## 6.5. Feasibility bằng Segment

Dùng:

```text
PrefixA + TailB
PrefixB + TailA
```

và `Segment::merge()` để kiểm tra:

- time windows;
- ready/latest compatibility;
- duration;
- max load;
- route cost.

Nếu:

\[
Q_a\neq Q_b,
\]

phải kiểm tra:

\[
maxLoad(Tail_B)\le Q_a
\]

và:

\[
maxLoad(Tail_A)\le Q_b.
\]

Không được giả sử tail feasible trên taxi cũ thì feasible trên taxi mới.

---

## 6.6. Complexity

Nếu route A có `C_a` clean cuts và route B có `C_b` clean cuts:

\[
O(C_a C_b)
\]

candidate pairs.

Mỗi candidate:

\[
O(1)
\]

nếu Segment merge / feasibility là `O(1)`.

---

## 6.7. Search strategy

Không cần full scan tất cả route pair ở mọi LS call.

### Tier 1

Chỉ route đã dirty:

```text
dirty route × all routes
```

### Tier 2

Route-pair candidate list theo:

- spatial proximity;
- compatible time horizon;
- potential reconnect lower bound.

### Tier 3

Full clean-cut 2-opt* khi:

- có new incumbent;
- stagnation;
- hoặc mỗi `X` giây.

---

## 6.8. Ưu tiên so với SWAP*

Clean-cut 2-opt* nên được xếp **trên SWAP*** vì:

- mỗi move thay cả tail lớn;
- đánh giá rất rẻ;
- giữ nguyên request integrity;
- tận dụng trực tiếp `Segment`;
- có khả năng vượt basin mà relocate đơn không vượt được.

---

# 7. Priority 4 — Blocker Removal và Time-Bucket Removal

## 7.1. Blocker Removal

Mục tiêu:

> Remove các request đang làm một request tốt khác không thể insert được.

Thay vì dùng công thức conflict chung chung, xác định blocker trực tiếp từ insertion failure.

Ví dụ với unassigned request `u`:

1. lấy các vị trí insertion promising theo cost;
2. nếu infeasible:
   - xác định constraint gây fail:
     - time;
     - capacity;
     - passenger boundary;
3. thu thập request đang gây blocker;
4. tăng blocker score cho request đó.

Pseudo-score:

```text
blocker_score[r] += lost_profit(u)
```

hoặc:

```text
blocker_score[r] += 1
```

weighted bởi:

```text
best_possible_gain(u)
```

Sau đó remove top blocker theo randomized rank.

---

## 7.2. Time-Bucket Removal

Chia timeline thành bucket:

```text
[0,B), [B,2B), ...
```

Mỗi request được map vào bucket theo:

- passenger pickup TW;
- parcel pickup TW;
- parcel delivery TW.

Chọn random / bad bucket rồi remove các request tập trung trong vùng đó.

Mục đích:

- phá local congestion theo thời gian;
- giải phóng một vùng TW bị chật;
- khác Shaw removal thuần spatial.

---

# 8. Priority 5 — LS Gating và Multi-Tier Local Search

## 8.1. Không chạy LS cho mọi candidate

Chỉ chạy LS khi candidate đủ tốt.

Ví dụ:

\[
Benefit(candidate)
\ge
BestBenefit-\delta.
\]

Hoặc normalized:

\[
\frac{Best-candidate}{|Best|+1}
\le \epsilon.
\]

---

## 8.2. Ba tầng LS

### Cheap LS

Chạy thường xuyên:

- Passenger Relocate;
- Parcel Relocate;
- Replace;
- cheap intra-route moves.

### Medium LS

Chạy khi candidate gần incumbent:

- clean-cut 2-opt*;
- restricted swap;
- block relocate.

### Expensive LS

Chạy hiếm:

- full SWAP*;
- large route exchange;
- set-packing polishing.

---

## 8.3. Adaptive operator scheduling theo hiệu quả thời gian

Với mỗi LS operator lưu:

```text
calls
cpu_time
num_improvements
total_gain
best_gain
```

Metric:

\[
Efficiency(op)
=
\frac{total\_gain}{cpu\_time+\epsilon}.
\]

Có thể kết hợp success rate:

\[
Score(op)
=
\alpha
\frac{gain}{sec}
+
\beta
\frac{improvements}{calls}.
\]

Dùng score để quyết định:

- frequency;
- tier;
- temporary disable;
- trigger khi stagnation.

---

# 9. Priority 6 — Regret Variants

## 9.1. Các regret cần có

### Global Regret-2

\[
Regret_2^G
=
gain_1-gain_2
\]

trên mọi insertion descriptor.

### Global Regret-3

Một lựa chọn:

\[
Regret_3^G
=
(gain_1-gain_2)
+
(gain_1-gain_3).
\]

### Different-route Regret-2

Second best phải thuộc route khác route của best insertion.

### Different-route Regret-3

Top candidate trên ba route khác nhau nếu có.

---

## 9.2. Không chỉ lưu top-3 descriptor global

Top-3 global không đủ để suy ra regret theo distinct route.

Ví dụ:

```text
A: 100
A: 99
A: 98
B: 70
```

top-3 global không chứa `B:70`.

Cần summary:

```cpp
struct InsertionSummary {
    Descriptor global_top[3];
    Descriptor best_per_route_top[3];
};
```

hoặc:

```text
best descriptor per route
+
global top-3
```

---

## 9.3. ALNS variants

Giữ operator riêng:

```text
GreedyInsertion
GlobalRegret2
GlobalRegret3
RouteRegret2
RouteRegret3
```

và cho adaptive weights học hiệu quả thực nghiệm.

---

# 10. Route Removal — Chỉnh lại mục tiêu

## 10.1. Không mô tả là "minimize active fleet"

Objective không có fixed fleet cost.

Tuy nhiên taxi được dùng phải chịu chi phí:

\[
c(O_k,v_1),
\]

nên mỗi active route có một location-dependent setup cost ngầm.

Vì vậy remove route ngắn vẫn có lý do.

---

## 10.2. Các score nên thử

### Revenue efficiency

\[
score_1(R)
=
\frac{Cost(R)}
{\epsilon+\sum_{r\in R}Revenue(r)}.
\]

### Net loss

\[
score_2(R)
=
Cost(R)-\sum_{r\in R}Revenue(r).
\]

### Origin consolidation pressure

\[
score_3(R)
=
\frac{c(O_k,v_1)}
{\max(1,|requests(R)|)}.
\]

Giữ nhiều RouteRemoval variants thay vì một score duy nhất.

---

## 10.3. Bug guard

Không được:

```cpp
1.0 / route.size()
```

khi route rỗng.

Chỉ sample trên:

```text
active non-empty routes
```

---

# 11. Related Removal — Cập nhật vừa phải

Relatedness hiện tại nên bổ sung:

- pickup proximity;
- drop-off proximity;
- earliest pickup difference;
- latest pickup difference;
- parcel delivery TW;
- request type compatibility.

Ví dụ:

\[
Rel(i,j)
=
\alpha d(P_i,P_j)
+
\beta d(D_i,D_j)
+
\gamma |E_i-E_j|
+
\delta |L_i-L_j|.
\]

Với parcel có thể thêm:

\[
\eta |E_i^d-E_j^d|
+
\theta |L_i^d-L_j^d|.
\]

Không ưu tiên cao bằng Blocker Removal.

---

# 12. Block Relocate

Chỉ cho phép block nếu block chứa request trọn vẹn.

Ví dụ block hợp lệ:

```text
parcel pickup
passenger pickup
passenger drop-off
parcel delivery
```

nếu parcel pickup và delivery đều nằm trong block.

Block không hợp lệ:

```text
parcel pickup
...
CUT
...
parcel delivery
```

Có thể thử block length nhỏ:

```text
2–4 logical requests
```

sau khi clean-cut 2-opt* ổn định.

---

# 13. Route Pool + Set Packing

Đây là hướng polishing có tiềm năng lớn sau khi ALNS đã mạnh.

## 13.1. Route pool

Trong quá trình search, lưu các route tốt:

```text
route signature
served request set
cost
taxi compatibility
timing feasibility
```

Deduplicate theo:

```text
(request set, taxi)
```

hoặc route hash.

---

## 13.2. Set packing master

Binary variable:

\[
x_r \in \{0,1\}
\]

cho mỗi route candidate.

Objective:

\[
\max
\sum_r
Benefit(r)x_r.
\]

Mỗi taxi chọn tối đa một route:

\[
\sum_{r\in R_k}x_r\le 1.
\]

Mỗi request phục vụ tối đa một lần:

\[
\sum_{r\ni q}x_r\le 1.
\]

---

## 13.3. Solver

Nếu competition cho phép CPLEX:

- dùng CPLEX trực tiếp.

Nếu không:

- custom branch-and-bound;
- Lagrangian relaxation;
- greedy + local improvement;
- maximum-weight set packing heuristic.

---

# 14. Balance Score theo thời gian chạy

Nếu solver có nhiều component cạnh tranh CPU, mọi adaptive score nên normalize theo time.

Không chỉ:

```text
operator produced improvement
```

mà nên dùng:

\[
Reward
=
\frac{\Delta Benefit}
{CPUTime+\epsilon}.
\]

Điều này đặc biệt quan trọng khi so:

- Relocate;
- clean-cut 2-opt*;
- SWAP*;
- set packing.

---

# 15. Instrumentation cần bổ sung

Cho mỗi operator lưu:

```text
calls
accepted
improved_current
improved_best
total_gain
best_gain
cpu_time
avg_time
candidates_tested
feasible_candidates
pruned_candidates
```

Riêng parcel insertion:

```text
pairs_theoretical
heap_pops
feasible_pop_rank
suffix_queries
adjacent_cases
```

Riêng 2-opt*:

```text
clean_cuts
cut_pairs_tested
capacity_pruned
time_pruned
improvements
```

---

# 16. Benchmark / Ablation Plan

Không đánh giá bằng một instance.

Chạy cùng seed set.

## Baseline

```text
B0 = current solver
```

## Incremental versions

```text
B1 = B0 + Segment fuzz fixes
B2 = B1 + Marginal Worst Removal
B3 = B2 + Exact heap parcel insertion
B4 = B3 + Clean-cut 2-opt*
B5 = B4 + Blocker + time-bucket removal
B6 = B5 + LS gating/tiering
B7 = B6 + regret variants
```

Theo dõi:

- best benefit;
- mean benefit;
- median benefit;
- runtime;
- time-to-best;
- number of served passenger;
- number of served parcel;
- active taxis;
- total route cost;
- total revenue.

---

# 17. Go / No-Go Criteria

Một feature được giữ nếu một trong các điều kiện sau đúng:

1. cải thiện mean objective có ý nghĩa;
2. cải thiện tail / hard instances;
3. giảm runtime đáng kể mà quality không giảm;
4. tăng robustness qua nhiều seed.

Feature nên bỏ hoặc giảm tần suất nếu:

- CPU cost cao;
- contribution gần 0;
- improvement chỉ trùng với operator khác;
- làm tăng variance không có benefit rõ.

---

# 18. Những hạng mục hiện chưa ưu tiên

## Long-span parcel removal

Ưu tiên thấp vì:

- parcel delivery window tương đối hẹp;
- thời gian parcel ở trên xe đã bị giới hạn;
- span theo node phần lớn phản ánh capacity interaction;
- Blocker / bundle removal xử lý trực tiếp hơn.

## Multi-start initial solution

Ưu tiên thấp khi:

- SA chạy đủ lâu;
- có reheating;
- search xóa mạnh ảnh hưởng initial solution.

Chỉ đáng bật nếu:

- time limit ngắn;
- hoặc initial phase chiếm tỷ lệ runtime nhỏ.

## Tách hoàn toàn Passenger/Parcel insertion engine

Chỉ làm khi profiling cho thấy abstraction chung gây overhead đáng kể.

---

# 19. Kiến trúc LS đề xuất sau cập nhật

```text
ALNS iteration
    |
    +-- Destroy
    |     Random
    |     WorstTravel
    |     WorstMarginalBenefit
    |     Related
    |     RouteRemoval
    |     BlockerRemoval
    |     TimeBucketRemoval
    |
    +-- Repair
    |     Greedy
    |     GlobalRegret2
    |     GlobalRegret3
    |     RouteRegret2
    |     RouteRegret3
    |
    +-- SA acceptance
    |
    +-- LS gating
          |
          +-- Cheap tier
          |     PassengerRelocate
          |     ParcelRelocate
          |     Replace
          |
          +-- Medium tier
          |     CleanCut2OptStar
          |     RestrictedSwap
          |     BlockRelocate
          |
          +-- Expensive tier
                FullSwapStar
                RoutePoolSetPacking
```

---

# 20. Immediate Implementation Checklist

## Phase A — Correctness

- [ ] Property test `Segment::merge`.
- [ ] Associativity fuzz test.
- [ ] FastRouteCheck vs full simulation.
- [ ] Intra-route relocate evaluated on `route_without_v`.
- [ ] Guard all empty-route divisions.
- [ ] Synchronize `architecture.md` with current time-based SA + reheating.

## Phase B — High ROI

- [ ] Add `WorstMarginalBenefitRemoval`.
- [ ] Add exact parcel delta decomposition.
- [ ] Implement adjacency parcel insertion case.
- [ ] Implement cost-ordered heap enumeration.
- [ ] Add suffix k-th-smallest data structure if profiling justifies it.

## Phase C — Structural LS

- [ ] Detect and cache clean cuts.
- [ ] Implement inter-route clean-cut 2-opt*.
- [ ] Handle open-route endpoint cuts.
- [ ] Handle heterogeneous `Q_k`.
- [ ] Add dirty-route incremental updates.

## Phase D — Better ALNS diversity

- [ ] Blocker Removal.
- [ ] Time-Bucket Removal.
- [ ] RelatedRemoval with pickup + drop-off + latest TW.

## Phase E — Search control

- [ ] LS gating near current/best.
- [ ] Cheap / medium / expensive tiers.
- [ ] Measure gain/second.
- [ ] Adapt LS frequencies from runtime statistics.

## Phase F — Repair diversification

- [ ] Global Regret-2.
- [ ] Global Regret-3.
- [ ] Different-route Regret-2.
- [ ] Different-route Regret-3.
- [ ] Cache best-per-route insertion descriptors.

## Phase G — Polishing

- [ ] Route pool.
- [ ] Route deduplication.
- [ ] Set-packing master.
- [ ] CPLEX path if permitted.
- [ ] Heuristic set-packing fallback otherwise.

---

# 21. Recommended Priority Order

Cuối cùng, thứ tự triển khai đề xuất:

1. **Segment property/fuzz tests**.
2. **Worst Removal theo marginal benefit**.
3. **Exact parcel insertion bằng heap enumeration**.
4. **Clean-cut 2-opt\***.
5. **Blocker Removal + Time-Bucket Removal**.
6. **LS gating + multi-tier scheduling**.
7. **Regret global / distinct-route / regret-3**.
8. **Route pool + set packing**.
9. Chỉ sau đó mới đầu tư thêm vào **full SWAP\*** hoặc các neighborhood đắt khác.

Trọng tâm kỹ thuật của phiên bản tiếp theo nên là:

\[
\boxed{
\text{Correct Segment}
\rightarrow
\text{Exact Fast Parcel Insertion}
\rightarrow
\text{Clean-Cut Route Exchange}
}
\]

Ba phần này có khả năng mang lại tỷ lệ **solution quality / runtime** lớn nhất trên kiến trúc hiện tại.
