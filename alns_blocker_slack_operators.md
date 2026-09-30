# Toán tử ALNS mới cho SoICT2026: Blocker Removal & Slack-Aware Insertion

Tài liệu mô tả hai toán tử (một destroy, một repair) và bộ đánh giá dùng chung với local search Replace. Tên hàm/kiểu dữ liệu là giả định, cần khớp lại với `SearchState`, `RouteCache`, `FastRouteCheck`, `InsertionSearch` của dự án.

Mục tiêu bài toán: **maximize** `Σ revenue(request được phục vụ) − Σ chi phí route`.

---

## 0. Ký hiệu và giả định

| Ký hiệu | Ý nghĩa |
|---|---|
| `K` | số taxi; `R_k = (R_k[1..L_k])` là dãy node dịch vụ của taxi `k` (không tính node xuất phát `O_k`) |
| `rev(x)` | doanh thu của request `x` |
| `p_x, d_x` | node pickup / drop-off của `x` |
| `owner(v)` | request sở hữu node `v` |
| `t(a,b), c(a,b)` | thời gian / chi phí đi từ `a` đến `b` (ma trận có hướng, có nhiễu, tam giác bất đẳng thức chỉ đúng gần đúng) |
| `e_v, l_v, svc_v` | earliest, latest, thời gian phục vụ tại node `v` |
| `Seg(k,a,b)` | `Segment` của `R_k[a..b]`, truy vấn O(1) từ DST |
| `Seg.cost` | tổng chi phí cung trong đoạn |
| `Seg.ready` | thời điểm sớm nhất rời node cuối của đoạn |
| `LI(k,a)` | `latest_input` của **hậu tố** `R_k[a..L_k]`: thời điểm muộn nhất đến `R_k[a]` mà không vi phạm cửa sổ nào phía sau |
| `bank` | tập request chưa được phục vụ (unassigned) |
| `B` | tập request bị gỡ (blockers) |
| `END` | vị trí sau node cuối của route |

Giả định:
- Một taxi chở tối đa **một passenger** tại một thời điểm (passenger không lồng nhau); parcel có thể lồng trong chuyến passenger, tổng trọng lượng ≤ `parcel_capacity`.
- Phiên bản đầu chỉ chèn `p_u` và `d_u` **liền kề nhau** (bỏ qua chèn xen kẽ cho parcel).
- Các đoạn dưới đây đánh chỉ số từ 1; `R_k[0]` là node xuất phát `O_k`.

---

## 1. Bộ đánh giá dùng chung

### 1.1 `DirectServe(k, i, u)` — chèn cặp `(p_u, d_u)` liền kề ngay sau `R_k[i]`

```
Input : taxi k, vị trí neo i (0 ≤ i ≤ L_k), request u
Output: (ok, dep_d)   -- ok: khả thi cục bộ; dep_d: thời điểm rời d_u

1  dep_i ← Seg(k,1,i).ready            (nếu i = 0 thì dep_i ← 0)
2  arr_p ← dep_i + t(R_k[i], p_u)
3  if arr_p > l[p_u] then return (false, –)
4  dep_p ← max(arr_p, e[p_u]) + svc[p_u]
5  arr_d ← dep_p + t(p_u, d_u)
6  if u là parcel then
7      if arr_d > l[d_u] then return (false, –)
8      start_d ← max(arr_d, e[d_u])
9  else
10     start_d ← arr_d                 -- passenger drop-off không có cửa sổ
11 dep_d ← start_d + svc[d_u]
12 return (true, dep_d)
```

Kiểm tra tải/parcel và quy tắc passenger được làm ở `BoundaryOK` (ước lượng) và `Validate` (chính xác).

### 1.2 `BoundaryOK(k, s, B, u)` — vị trí cắt có hợp lệ không

```
Input : taxi k, chỉ số node kế tiếp s, tập bị gỡ B, request u
Output: true/false

1  if u là passenger then
2      return không có passenger nào khác (ngoài B) đang trên xe tại khe [s-1, s]
3  else  -- u là parcel
4      return tải parcel trên xe tại khe [s-1, s] (trừ các parcel thuộc B) + w_u ≤ Q_k
```
Đây là ước lượng bảo thủ dựa trên tải prefix của route gốc; kết quả chính xác do `Validate` quyết định.

### 1.3 `ReplaceEval(k, i, m*, u, B)` — tính chính xác Δbenefit của phép thay `B → u`

Khối bị thay là `R_k[i+1 .. m*-1]`, gồm **toàn bộ node thuộc `B`** (do cách dựng ở §2). Công thức:

```
Δbenefit = rev(u) − Σ_{b∈B} rev(b)
           − [ c(R_k[i], p_u) + c(p_u, d_u) + c(d_u, R_k[m*]) ]      -- chi phí cung mới
           + Seg(k, i, m*).cost                                       -- chi phí cung cũ
```
(với `m* = END` thì bỏ số hạng `c(d_u, ·)` và lấy chi phí cũ đến node cuối.)

Tính chất:
- Chính xác về chi phí khi mọi node của `B` nằm trong khối. Nếu `B` còn node nằm ngoài khối (ví dụ drop-off của một parcel ở sau `m*`) thì gỡ chúng chỉ **thêm tiết kiệm**, nên công thức là **cận dưới an toàn** (khi tam giác bất đẳng thức gần đúng).
- Tính khả thi về thời gian không nằm trong công thức; nó được đảm bảo bởi `Validate` ở bước commit.

### 1.4 `Validate(route')` — bộ kiểm tra cuối cùng

Chạy đầy đủ time window, tải, quy tắc passenger trên một **bản sao route k** đã thay đổi (O(L_k)). Mọi phép commit đều phải qua bước này.

---

## 2. Destroy: Blocker Removal

### 2.1 Ý tưởng

Chọn một khách `u` trong bank. Với mỗi taxi và mỗi vị trí neo, tìm tập request tối thiểu đang chắn chỗ của `u`, tính chính xác Δbenefit của phép "gỡ `B`, chèn `u`". Nếu có nước đi cải thiện thì áp dụng ngay; ngược lại chọn theo hạng lệch trong top-K. Sau khi gỡ, `u` được chèn luôn (ejection chain), `B` vào bank cho repair.

### 2.2 Tham số

| Tham số | Giá trị khởi điểm | Ý nghĩa |
|---|---|---|
| `q` | ≤ 10% số request đang phục vụ | số request tối đa được gỡ mỗi lần gọi |
| `W` | 3–5 | số blocker tối đa cho một target |
| `topK` | 3 | chọn ngẫu nhiên lệch hạng trong top-K theo Δbenefit |
| `maxTries` | 3 | số target thử mỗi lần gọi |
| `ε` | 0.05 | sàn trọng số chọn target |
| `α` | 0.1 | hệ số EWMA cho tỉ lệ mở khóa `s_u` |

### 2.3 Tiền xử lý (một lần)

```
feasible_alone[u] ← tồn tại taxi k: t(O_k, p_u) ≤ l[p_u]
s[u] ← 1 với mọi u         -- tỉ lệ mở khóa thành công (EWMA)
```

### 2.4 Chọn target

```
SelectTarget(bank):
1  C ← { u ∈ bank : feasible_alone[u] }
2  nếu C rỗng: return NIL
3  w_u ← (ε + s[u]) · rev(u)   với u ∈ C           -- luôn > 0
4  return u lấy mẫu theo w
```
Không dùng `age` (khuếch đại xác suất của request vốn kém) và không loại target theo lợi nhuận tĩnh `rev − c(p_u,d_u)`.

### 2.5 Thuật toán chính

```
Algorithm 1: BlockerRemoval(S, q, W, topK, maxTries)
Input : S (SearchState), tham số
Output: danh sách request được đẩy vào bank (removed); u có thể đã được chèn

1   removed ← ∅
2   for try = 1 .. maxTries while |removed| < q do
3       u ← SelectTarget(S.bank)
4       if u = NIL then break
5       Cand ← ∅
6       for k = 1 .. K do
7           for i = 0 .. L_k do
8               (ok, dep_d) ← DirectServe(k, i, u)
9               if not ok then continue
10              B ← ∅ ; s ← i + 1 ; fail ← false
11              loop
12                  s ← NextKept(R_k, s, B)        -- bỏ qua node thuộc request trong B
13                  if s = END then break          -- m* = END
14                  arr ← dep_d + t(d_u, R_k[s])
15                  if arr ≤ LI(k, s) and BoundaryOK(k, s, B, u) then break   -- m* = s
16                  if |B| = W then fail ← true ; break
17                  B ← B ∪ { owner(R_k[s]) }
18              end loop
19              if fail or B = ∅ then continue
20              if node neo R_k[i] thuộc request trong B then continue        -- neo bị gỡ
21              Δ ← ReplaceEval(k, i, s, u, B)
22              Cand ← Cand ∪ { (k, i, s, B, Δ) }
23          end for
24      end for
25      if Cand = ∅ then s[u] ← (1−α)·s[u] ; continue
26      if max Δ trong Cand > 0 then c ← ứng viên có Δ lớn nhất
27      else c ← lấy mẫu lệch hạng trong topK ứng viên theo Δ giảm dần
28      if Commit(S, c, u) then
29          removed ← removed ∪ c.B
30          s[u] ← (1−α)·s[u] + α
31      else
32          s[u] ← (1−α)·s[u]
33  end for
34  return removed
```

`NextKept(R_k, s, B)` trả về chỉ số node đầu tiên ≥ `s` mà `owner` không thuộc `B` (hoặc `END`). Nhờ đó vòng lặp không bao giờ kiểm tra một node đã bị gỡ, và **mọi node trong khối `(i, m*)` đều thuộc `B`**.

### 2.6 Commit với dry-run

```
Algorithm 1a: Commit(S, c = (k, i, m*, B, Δ), u)
1   R' ← bản sao của R_k
2   gỡ mọi node của request trong B khỏi R'      -- B thuộc cùng route k
3   chèn p_u, d_u liền kề sau neo (tương ứng vị trí i) trong R'
4   if not Validate(R') then return false        -- ước lượng sai, tính vào tỉ lệ thất bại
5   S.remove(B)                                   -- đưa B vào bank
6   S.insert(u, k, vị trí đã chọn)
7   invalidate cache của taxi k
8   return true
```

### 2.7 Độ phức tạp và tác động lên cache

- Mỗi target: `O(K · L · W)` truy vấn O(1) (DST) + `O(L)` cho commit.
- Chỉ chạm **một route** mỗi lần commit, nên ít làm mất hiệu lực `RouteCache`/`BestInsertionCache`.

### 2.8 Lưu ý

- `LI(k, s)` lấy từ cache route gốc là **ước lượng** (gỡ `B` ở prefix hay suffix làm đổi giá trị thật); đó là lý do bắt buộc có dry-run.
- Ma trận nhiễu có thể vi phạm tam giác bất đẳng thức nhẹ, khiến Δbenefit lệch nhỏ.
- Nếu `Δ > 0` (dòng 26) thì đây đã là một nước cải thiện thật; có thể áp dụng độc lập như local search (xem §4).

---

## 3. Repair: Slack-Aware Insertion

### 3.1 Ý tưởng

Greedy xếp hạng theo `gain = rev − Δcost`, không phản ánh việc phép chèn có làm cạn slack thời gian của route hay không (thứ quyết định khả năng chèn các request sau). Toán tử này xếp hạng theo `score = gain − pen`, trong đó `pen ≥ 0` phạt việc thu hẹp slack. Quyết định phục vụ hay từ chối vẫn dựa trên **gain thật**.

### 3.2 Tham số

| Tham số | Giá trị khởi điểm | Ý nghĩa |
|---|---|---|
| `θ` | 0 (hoặc ngưỡng theo SA) | ngưỡng gain tối thiểu để chấp nhận chèn |
| `κ` | {0, 0.25, 0.5} | hệ số nhân của `λ = κ·λ₀` |
| `τ` | ≈ 30 phút | ngưỡng slack của hàm chắn |
| `λ₀` | median của `(rev − direct_cost)/max(direct_time, ε_t)` | lợi nhuận quy đổi theo phút, tính một lần |

### 3.3 Hàm phạt

Với ứng viên `(k, i, j)` (pickup sau `R_k[i]`, drop-off sau `R_k[j]`, `j ≥ i`), gọi `y = j+1` là node đầu tiên sau drop-off. Slack của một đoạn hậu tố/khoảng bắt đầu tại `a`: `σ(a) = LI(·) − arr_a`. Định nghĩa

```
σ_mid_before = LI(k, i+1..j) − arr_old[i+1]           (khoảng giữa pickup và drop-off; +∞ nếu rỗng)
σ_mid_after  = LI(k, i+1..j) − arr_new[i+1]
σ_suf_before = LI(k, y)       − arr_old[y]            (+∞ nếu y = END)
σ_suf_after  = LI(k, y)       − arr_new[y]
σ_before = min(σ_mid_before, σ_suf_before)
σ_after  = min(σ_mid_after,  σ_suf_after)
```

Ba biến thể penalty (nên đo song song):

| Biến thể | Công thức `pen` |
|---|---|
| **A: push** | `λ · max(0, dep_new[y] − dep_old[y])`; `pen = 0` nếu `y = END` |
| **B: push chuẩn hóa** | `λ · max(0, dep_new[y] − dep_old[y]) / (1 + σ_before)` |
| **C: hàm chắn** | `λ · max(0, φ(σ_after) − φ(σ_before))`, với `φ(σ) = max(0, τ − σ)` |

Cuối route (`y = END`): `pen = 0`, vì không có node nào phía sau để bị chắn.
Luôn kẹp `pen ← max(0, pen)`. Ma trận nhiễu có thể khiến dep_new < dep_old trong trường hợp hiếm; kẹp đảm bảo `score ≤ gain` và bộ lọc cận trên hiện có vẫn hợp lệ.

### 3.4 Thuật toán

```
Algorithm 2: SlackAwareInsertion(S, λ, θ, penalty_type)
Input : S sau khi destroy (bank = tập cần chèn)
Output: S sau repair

1   while bank ≠ ∅ do
2       best ← NIL
3       for r ∈ bank do
4           c ← BestCandidate(S, r, λ, θ, best.score)
5           if c ≠ NIL and (best = NIL or c.score > best.score) then best ← c
6       end for
7       if best = NIL then break
8       Apply(S, best) ; bank ← bank \ { best.r }
9       invalidate cache của taxi best.k
10  end while
```

```
Algorithm 2a: BestCandidate(S, r, λ, θ, bound)
Input : bound = best.score hiện có (NIL → –∞)
Output: ứng viên có score lớn nhất và gain > θ, hoặc NIL

1   best ← NIL
2   duyệt (k, i, j) theo UB_gain giảm dần        -- UB từ cận dưới chi phí (cung kề), như InsertionSearch hiện tại
3       if UB_gain ≤ max(θ, bound, best.score) then break          -- cắt sớm (lazy)
4       if not FastRouteCheck(k, i, j, r) then continue
5       gain ← rev(r) − Δcost(k, i, j, r)
6       if gain ≤ θ then continue                                   -- cổng trên GAIN thật, ở mức ứng viên
7       score ← gain − pen(k, i, j, r)                              -- pen theo §3.3
8       if score > best.score then best ← (r, k, i, j, gain, score)
9   return best
```

Điểm mấu chốt: cổng `gain > θ` áp **trên từng ứng viên** (dòng 6), không áp lên ứng viên tốt nhất theo score, vì vị trí tốt nhất theo score không nhất thiết có gain lớn nhất.

### 3.5 Biến thể regret (tùy chọn)

Thay thứ tự chọn ở dòng 5 bằng regret giữa hai `score` tốt nhất **thuộc hai taxi khác nhau** của cùng request (nếu chỉ có một taxi khả thi, regret coi như vô cùng lớn).

### 3.6 Đăng ký toán tử và cache

- Đăng ký 3 toán tử riêng: `κ = 0, 0.25, 0.5` (mỗi toán tử một `BestInsertionCache`), để Thompson sampling/roulette tự chọn. `κ = 0` chính là greedy thường (giữ lại làm đối chứng).
- Cache theo taxi vẫn hợp lệ khi route taxi đó không đổi vì `pen` chỉ phụ thuộc route.
- Nếu `FastRouteCheck` chưa xuất được `ready` tại `y`, tách truy vấn `Seg(b, L)` thành `Seg(b, b)` (đọc `ready`) rồi phần còn lại.

### 3.7 Độ phức tạp

Cùng bậc với `GreedyInsertion` (`O(|bank| · K · L²)` trường hợp xấu nhất, giảm nhiều nhờ lọc UB và cache). Score nhỏ hơn gain nên ngưỡng cắt thấp hơn và ít ứng viên bị loại hơn; duyệt lười theo UB giảm dần dùng để bù.

---

## 4. Local search Replace (dùng chung bộ đánh giá)

Blocker Removal thực chất là Replace kiểu 1↔nhiều nhúng trong destroy. Nên cài Replace trước với **cùng** `DirectServe`, `ReplaceEval`, `Validate`:

```
Algorithm 3: ReplaceLS(S, W)
1   improved ← true
2   while improved do
3       improved ← false
4       for u ∈ S.bank (theo rev giảm dần) do
5           Cand ← các (k, i, m*, B, Δ) dựng như Algorithm 1 dòng 6–22, với |B| ≤ W
6           c ← ứng viên có Δ lớn nhất
7           if c ≠ NIL and c.Δ > 0 and Commit(S, c, u) then improved ← true
8       end for
9   end while
```
Thứ tự triển khai đề xuất: Replace 1↔1 (`W = 1`), sau đó mở rộng 1↔nhiều (`W ≤ 4`), sau đó nhúng vào destroy (Algorithm 1) cho phép nhận cả `Δ < 0` theo cổng SA.

---

## 5. Tích hợp với vòng lặp ALNS

- **Destroy set:** `Random`, `Worst`, `Related` (hiện có) + `BlockerRemoval`. Gắn nhãn `BlockerRemoval` là "ejection destroy" khi đọc thống kê, vì nó đã chèn `u` trong bước destroy.
- **Repair set:** `Greedy`, `Regret` (hiện có) + `SlackAware(κ=0)`, `SlackAware(κ=0.25)`, `SlackAware(κ=0.5)`.
- **Balance:** chuẩn hóa điểm theo thời gian chạy (điểm/giây) để không thiên vị toán tử nhanh.
- **`WorstRemoval` (sửa nhỏ):** chọn ngẫu nhiên lệch hạng `index = ⌊rand^p · n⌋` với `p ≈ 3–6` thay vì lấy `q` phần tử đầu.

---

## 6. Số liệu cần log

Cho mỗi toán tử: số lần gọi, số lần cải thiện, số best mới, số lần được SA chấp nhận, thời gian trung bình.
Riêng `BlockerRemoval`: **tỉ lệ mở khóa thành công** (commit qua `Validate` / số ứng viên được chọn), `s_u` theo từng request, phân bố `|B|`, tỉ lệ `Δ > 0`.
Riêng `SlackAware`: phân bố `pen`, tỉ lệ đổi thứ hạng so với `gain` thuần (ứng viên tốt nhất khác nhau), tỉ lệ ứng viên bị cắt sớm.

## 7. Kế hoạch kiểm thử

1. Instance kích thước 100, 250, 500 (đủ 5 tỉ lệ passenger:parcel), nhiều seed.
2. Ablation: baseline; +ReplaceLS; +BlockerRemoval; +SlackAware (3 biến thể penalty A/B/C); +tất cả.
3. So sánh theo cùng thời gian chạy: benefit tốt nhất trung bình, số request phục vụ, thời gian đến best.

## 8. Hạn chế và mở rộng

- Chưa xử lý chèn xen kẽ cho parcel (pickup và drop-off tách nhau); mở rộng bằng giới hạn khoảng cách `j − i`.
- Kiểm tra tải trong `BoundaryOK` là ước lượng; độ chính xác do `Validate` quyết định.
- `LI` từ cache gốc là ước lượng bảo thủ khi gỡ node ở prefix; Δbenefit là cận dưới an toàn, không phải giá trị đúng tuyệt đối trong mọi trường hợp.
- Tất cả tham số (`W`, `topK`, `τ`, `κ`) cần tinh chỉnh trên tập instance thử, không nên coi giá trị khởi điểm là tối ưu.
