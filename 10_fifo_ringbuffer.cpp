#include <iostream>
#include <cstddef>
#include <atomic>

/*
──────────────────────────────────────────────────────────────────────────────
[面試前備忘錄｜Google Embedded 視角]
• 目標：SPSC（Single Producer / Single Consumer）無鎖環形緩衝區（ring buffer）。
  - 單生產者只寫 head 與對應格子；單消費者只寫 tail 與對應格子 → 無需鎖。
  - O(1) push/pop；N 為 2 的冪 → 用位遮罩 & (N-1) 代替 modulo（更快）。
  - 使用 std::atomic + memory_order_acquire/release 保證可見性與順序。
• 記憶體序（超高頻追問）：
  - Producer: 先寫資料格，再以 release 發佈 head。
  - Consumer: 先以 acquire 讀 head/判斷，再讀資料格。
  - 這確保「看到 head」就保證能看到對應資料（HB happens-before）。
• MCU / RTOS 注意：
  - 多為單核：atomic 對 32-bit 存取通常原子；但 8-bit MCU 需留意自然對齊/位寬。
  - ISR ↔ main thread：這種 SPSC 適合 ISR 推資料、主迴圈取資料；可避免關中斷。
• 類型 T 的限制：
  - 以平凡可移動/拷貝型別（POD/TriviallyCopyable）最佳；複雜型別可行但需留意拷/移動成本。
  - 若需「多態或大物件」→ 放指標/索引/句柄，不要放大型物件本體。
──────────────────────────────────────────────────────────────────────────────
[常見追問（口條）]
Q1: 為什麼不用 % 而是 & (N-1)？
A1: N 為 2 的冪時，位遮罩更快且無除法延遲；是高頻 ISR 路徑的常見最佳化。

Q2: 為什麼 head 用 release、tail 用 release，而對側用 acquire？
A2: release 發佈更新，acquire 觀察更新並建立 HB 關係，確保資料寫入先於指標更新被觀察到。

Q3: 空／滿判斷？
A3: 「保留一格」法：當 (head+1)==tail 為滿；head==tail 為空。這裡用遮罩後的等價判斷。

Q4: 要不要 seq_cst？
A4: SPSC 下 acquire/release 已足夠；seq_cst 成本較高且不必要。

Q5: False sharing？
A5: 多核時可加 cache line padding（alignas(64)）隔離 head/tail；單核 MCU 通常不需要。
──────────────────────────────────────────────────────────────────────────────
[陷阱備忘]
• 千萬別在 MPMC 場景用這段：這是 SPSC 專用。
• 不要用有號型別當指標；用 size_t 並確保自然對齊。
• T 若非平凡可移動，可能有拷/移動成本；必要時存指標或使用 emplace。
• 8-bit / 16-bit MCU 要確保 head/tail 的原子性（可改用 irq disable 或 16/32-bit 原子序列）。
──────────────────────────────────────────────────────────────────────────────
*/

template <typename T, std::size_t N>
struct Ring {
    static_assert((N & (N - 1)) == 0, "N must be power of two"); // 2 的冪才能用位遮罩
    static constexpr std::size_t MASK = N - 1;

    // ⚠️ 原生陣列：無 heap；在 embedded 中決定性好
    T buf[N];

    // 單生產者只寫 head；單消費者只寫 tail
    std::atomic<std::size_t> head; // 下一個寫入位置（已使用格數的「末端」）
    std::atomic<std::size_t> tail; // 下一個讀取位置（未讀的「前端」）

    Ring() : head(0), tail(0) {
        // buf 無需初始化：push 時覆蓋
    }

    // 佇列容量（可用於上層配置/監控）
    static constexpr std::size_t capacity() noexcept { return N; }

    // 當前大小（近似，僅供監控；SPSC 下 h/t 在單線各自更新可安全取）
    std::size_t size() const noexcept {
        std::size_t h = head.load(std::memory_order_acquire);
        std::size_t t = tail.load(std::memory_order_acquire);
        return (h - t) & MASK; // 在遮罩空間內的距離
    }
    bool empty() const noexcept {
        return (head.load(std::memory_order_acquire) & MASK) ==
               (tail.load(std::memory_order_acquire) & MASK);
    }
    bool full() const noexcept {
        std::size_t h = head.load(std::memory_order_acquire);
        std::size_t t = tail.load(std::memory_order_acquire);
        return (((h + 1) & MASK) == (t & MASK));
    }
    void clear() noexcept {
        // 僅在無並發時呼叫；或上層保證 producer/consumer 暫停
        tail.store(head.load(std::memory_order_relaxed), std::memory_order_release);
    }

    // push：複製版本（傳 const&）
    bool push(const T& x) {
        // Producer side：讀自己寫的 head（relaxed 即可），讀對側 tail 要 acquire
        std::size_t h = head.load(std::memory_order_relaxed);
        std::size_t t = tail.load(std::memory_order_acquire);
        if (((h + 1) & MASK) == (t & MASK)) return false; // 滿

        buf[h & MASK] = x;                                 // 先寫資料
        head.store(h + 1, std::memory_order_release);      // 再發佈更新（HB）
        return true;
    }

    // push：移動版本（若 T 可移動，避免拷貝）
    bool push(T&& x) {
        std::size_t h = head.load(std::memory_order_relaxed);
        std::size_t t = tail.load(std::memory_order_acquire);
        if (((h + 1) & MASK) == (t & MASK)) return false;  // 滿

        buf[h & MASK] = std::move(x);
        head.store(h + 1, std::memory_order_release);
        return true;
    }

    // pop：取出一筆
    bool pop(T& out) {
        // Consumer side：讀自己寫的 tail（relaxed），讀對側 head 要 acquire
        std::size_t t = tail.load(std::memory_order_relaxed);
        std::size_t h = head.load(std::memory_order_acquire);
        if ((t & MASK) == (h & MASK)) return false;        // 空

        out = buf[t & MASK];                                // 讀資料
        tail.store(t + 1, std::memory_order_release);       // 發佈消費
        return true;
    }
};

int main() {
    Ring<int, 8> r; // 8（2 的冪）→ 掩碼 0b0111

    for (int i = 0; i < 6; ++i) {
        std::cout << "push " << i << " ok=" << std::boolalpha << r.push(i) << "\n";
    }
    std::cout << "size=" << r.size() << " empty=" << r.empty() << " full=" << r.full() << "\n";

    int x;
    while (r.pop(x)) {
        std::cout << "pop " << x << "\n";
    }
    std::cout << "size=" << r.size() << " empty=" << r.empty() << " full=" << r.full() << "\n";
    return 0;
}

/*
──────────────────────────────────────────────────────────────────────────────
[總結口條（可直接講）]
• 這是 SPSC 無鎖 ring buffer：N 為 2 的冪，用位遮罩 & (N-1) 加速索引。
• Producer：先寫 buf，再以 release 發佈 head；Consumer：以 acquire 看到 head 後再讀 buf。
• 空/滿：保留一格辨識；空=(head==tail)，滿=((head+1)==tail)（皆在遮罩空間判斷）。
• 嵌入式：不動用 heap、O(1)、可進 ISR；多核可加 cache line padding 降低 false sharing。
• 需要 MPMC 時不可沿用此實作，應改用鎖或專用無鎖演算法。
──────────────────────────────────────────────────────────────────────────────
*/
