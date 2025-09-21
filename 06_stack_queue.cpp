#include <iostream>
#include <stack>
#include <queue>
#include <array>
#include <cstddef>

/*
──────────────────────────────────────────────────────────────────────────────
[面試前備忘錄｜Google Embedded 視角]
• STL stack/queue 是「容器適配器」(adapters)：語義明確 (LIFO/FIFO)，預設底層為 deque。
• 嵌入式常見需求：固定容量 + 無 heap + O(1) 操作 → 使用環形佇列 (ring buffer)。
• API 設計：以回傳 bool 表示成功/失敗、不丟例外；提供 size/capacity/empty/full 便於監控。
• 效能/延遲：避免 reallocation（不可預測延遲）；固定大小結構更 deterministic。
• 多執行緒：單執行緒或 SPSC 簡單；多執行緒需鎖或無鎖原子（memory order）。
──────────────────────────────────────────────────────────────────────────────
[常見追問（口條提示）]
Q1: stack/queue vs vector/list？
A1: stack/queue 是 adapters，暴露有限 API（push/pop/top/front/back）。vector/list 是完整容器。

Q2: 為何用固定大小 queue？
A2: 避免 heap 碎片與不可預測延遲；可預先保證容量與時間界線（determinism）。

Q3: 如何判斷滿/空？
A3: 兩種典型法：計數器 (cnt) 或「保留一格」(head==tail 表示空、(tail+1)==head 表示滿)。

Q4: 2 的冪最佳化？
A4: 若 N 為 2 的冪，(i+1)%N 可改為 (i+1)&(N-1) 減少除法成本（ISR/高頻路徑常見）。
──────────────────────────────────────────────────────────────────────────────
[陷阱備忘]
• 別在空佇列上讀 front()/pop()；先檢查 empty() 或改用回傳 bool 的 pop(out)。
• 多執行緒時 head/tail 更新需同步（鎖或 atomic）。SPSC 可用 acquire/release。
• adapters 不提供迭代器；若要遍歷內容、請使用對應的底層容器或自訂容器。
──────────────────────────────────────────────────────────────────────────────
*/

/// ───────────────────────────────────────────────
/// STL 範例：stack / queue（容器適配器）
/// 時間複雜度：push/pop/top/front/back 均為 O(1)
/// ───────────────────────────────────────────────
void stl_demo() {
    std::stack<int> st;         // LIFO
    st.push(1);
    st.push(2);
    std::cout << "stack top=" << st.top() << "\n"; // 2
    st.pop();

    std::queue<int> q;          // FIFO
    q.push(10);
    q.push(20);
    std::cout << "queue front=" << q.front() << "\n"; // 10
    q.pop();
}

/// ───────────────────────────────────────────────
/// 固定大小佇列 (Ring Buffer) — 嵌入式友善：
/// • 編譯期容量 N；無 heap；push/pop O(1)；不會 reallocate
/// • 單執行緒或 SPSC（single-producer single-consumer）情境最簡潔
/// • 本實作非執行緒安全；多執行緒需鎖/atomic（見備忘）
/// API 語意：push/pop 回傳 bool 表示成功/失敗，不丟例外（RT 安全）
/// ───────────────────────────────────────────────
template <std::size_t N>
struct FixedQueue {
    static_assert(N > 0, "FixedQueue capacity must be > 0");

    std::array<int, N> buf{};        // 內部緩衝：放在物件內（通常 stack/靜態區）
    std::size_t head = 0;            // 下一個要讀的位置
    std::size_t tail = 0;            // 下一個要寫的位置
    std::size_t cnt  = 0;            // 當前元素數

    static constexpr std::size_t capacity() noexcept { return N; }

    bool push(int x) noexcept {
        if (full()) return false;            // 滿則拒絕（不覆蓋）
        buf[tail] = x;
        tail = (tail + 1) % N;               // 環形遞增
        ++cnt;
        return true;
    }

    bool pop(int& out) noexcept {
        if (empty()) return false;
        out = buf[head];
        head = (head + 1) % N;
        --cnt;
        return true;
    }

    // 觀察端點（呼叫者須先保證非空）
    int&       front()       noexcept { return buf[head]; }
    const int& front() const noexcept { return buf[head]; }
    int&       back()        noexcept { return buf[(tail + N - 1) % N]; }
    const int& back()  const noexcept { return buf[(tail + N - 1) % N]; }

    bool empty()    const noexcept { return cnt == 0; }
    bool full()     const noexcept { return cnt == N; }
    std::size_t size() const noexcept { return cnt; }
};

/*
──────────────────────────────────────────────────────────────────────────────
[延伸：2 的冪最佳化（可作口述，不一定要改 code）]
template <std::size_t N>
struct Pow2Queue {
    static_assert(N && ((N & (N - 1)) == 0), "N must be power of two");
    std::array<int, N> buf{};
    std::size_t head = 0, tail = 0;
    static constexpr std::size_t mask = N - 1;
    bool empty() const { return head == tail; }
    bool full()  const { return ((tail + 1) & mask) == head; } // 保留一格
    bool push(int x){ if(full()) return false; buf[tail]=x; tail=(tail+1)&mask; return true; }
    bool pop(int& o){ if(empty())return false; o=buf[head]; head=(head+1)&mask; return true; }
}
──────────────────────────────────────────────────────────────────────────────
[多執行緒備忘（SPSC 無鎖思路）]
• head 只由 consumer 更新，tail 只由 producer 更新。
• 使用 std::atomic<size_t> 與 memory_order_acquire/release 保證可見性。
• MPMC 情境建議採用現成無鎖佇列或加鎖，避免自研踩坑。
──────────────────────────────────────────────────────────────────────────────
*/

int main() {
    stl_demo();

    FixedQueue<4> fq;  // 固定容量 = 4（無 heap，O(1)）
    for (int i = 0; i < 4; ++i) {
        std::cout << "push " << i << " ok? " << std::boolalpha << fq.push(i) << "\n";
    }
    std::cout << "full? " << fq.full() << ", size=" << fq.size() << "\n";
    std::cout << "front=" << fq.front() << ", back=" << fq.back() << "\n";

    int x = 0;
    while (fq.pop(x)) {
        std::cout << "pop " << x << ", size now=" << fq.size() << "\n";
    }
    std::cout << "empty? " << fq.empty() << "\n";

    return 0;
}
