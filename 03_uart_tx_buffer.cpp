#include <array>
#include <atomic>
#include <iostream>

/*
──────────────────────────────────────────────────────────────
[面試前備忘錄｜I/O blocker / 限流（UART 例）]
• I/O 慢於 CPU：直接 write 可能阻塞（latency 不可控）。
• 解法：用 ring buffer（FIFO）緩衝；ISR/DMA 慢慢送，App 不被 block。
• API 設計：
  - non-blocking：立即回傳是否成功（滿則 false）；呼叫端可選擇 retry/丟棄。
  - blocking：busy-wait 等待空間（❌ ISR 禁用；RT loop 也不建議）。
• Flow control 策略：
  - drop-new（本範例）：滿了丟最新輸入，保留已在隊列中的舊資料。
  - drop-old：覆蓋最舊的（保留最新）；或提供策略參數讓上層選擇。
• 記憶體序（單生產者/單消費者 SPSC）：
  - Producer：先寫資料格，再以 release 發佈 head。
  - Consumer：先以 acquire 讀 head，確保看到資料，再讀資料格；消費後以 release 發佈 tail。
──────────────────────────────────────────────────────────────
[常見追問（口條）]
Q: 為什麼不用 mutex？
A: SPSC 下各自只寫 head/tail，可用 atomic + acquire/release 保證順序與可見性，零鎖開銷。

Q: 滿了怎麼辦？
A: 策略取決於需求：drop-new / drop-old / blocking。這裡採 drop-new + overflow 計數，便於監控。

Q: 為什麼用 & (N-1) 而不用 % N？
A: 若 N 為 2 的冪，位遮罩更快；本例用 % 也可（簡潔清楚）。要極致效能時再換遮罩。

Q: ISR 能用 blocking 嗎？
A: 不行。ISR 應該非阻塞最小化工作；blocking 只留在非 ISR 的情境。
──────────────────────────────────────────────────────────────
[陷阱備忘]
• 多生產者/多消費者就不是這個解法了：需要鎖或更複雜的 lock-free 演算法。
• ISR/中斷中不要動態配置或呼叫可能阻塞的 API。
• 容量太小會頻繁 overflow；至少提供 dropped 計數協助診斷。
──────────────────────────────────────────────────────────────
*/

struct UartTx {
    static const std::size_t N = 16;             // 小容量，便於演示「易滿 → 限流」

    std::array<char, N> buf;                     // C++11：不在此處 brace-init（避免 NSDMI 需求）
    std::atomic<std::size_t> head;               // Producer: 下一個寫入索引
    std::atomic<std::size_t> tail;               // Consumer: 下一個讀取索引
    std::atomic<std::size_t> dropped;            // overflow 計數器（debug/監控）

    UartTx() : head(0), tail(0), dropped(0) {
        // 可選：buf.fill(0);
    }

    // 非阻塞寫：滿了直接返回 false（drop-new 策略）
    bool write_nonblocking(char c) {
        std::size_t h = head.load(std::memory_order_relaxed);
        std::size_t t = tail.load(std::memory_order_acquire);
        if (((h + 1) % N) == t) {
            // 滿：不寫入，統計 overflow
            dropped.fetch_add(1, std::memory_order_relaxed);
            return false;
        }
        buf[h] = c;  // 先寫資料
        head.store((h + 1) % N, std::memory_order_release); // 再發佈 head（HB）
        return true;
    }

    // 阻塞寫：忙等直到有空間（❌ 不可在 ISR 內使用）
    void write_blocking(char c) {
        for (;;) {
            std::size_t h = head.load(std::memory_order_relaxed);
            std::size_t t = tail.load(std::memory_order_acquire);
            if (((h + 1) % N) != t) {
                buf[h] = c;
                head.store((h + 1) % N, std::memory_order_release);
                return;
            }
            // busy-wait：在 RT 系統通常會改成事件/旗號喚醒，而非空轉
        }
    }

    // 消費端讀一個字元；空則回傳 false
    bool read(char& out) {
        std::size_t t = tail.load(std::memory_order_relaxed);
        std::size_t h = head.load(std::memory_order_acquire);
        if (t == h) return false;    // empty
        out = buf[t];
        tail.store((t + 1) % N, std::memory_order_release);
        return true;
    }

    // 便利函式：狀態查詢（僅監控用途）
    bool empty() const {
        return head.load(std::memory_order_acquire) ==
               tail.load(std::memory_order_acquire);
    }
    bool full() const {
        std::size_t h = head.load(std::memory_order_acquire);
        std::size_t t = tail.load(std::memory_order_acquire);
        return ((h + 1) % N) == t;
    }
};

int main() {
    UartTx tx;

    // 測試 non-blocking：嘗試塞超過容量以觸發 overflow
    for (int i = 0; i < 20; ++i) {
        if (!tx.write_nonblocking(static_cast<char>('A' + (i % 26)))) {
            std::cout << "[overflow] drop char\n";
        }
    }

    // 模擬消費端把資料取出
    char c;
    while (tx.read(c)) {
        std::cout << c << ' ';
    }
    std::cout << "\n";
    std::cout << "dropped=" << tx.dropped.load(std::memory_order_relaxed) << "\n";

    /*
    ───────────────────────────────────────────────────────────
    [總結口條（可直接講）]
    • 我用 SPSC ring 避免 I/O 阻塞；提供 non-blocking 與 blocking 兩種 API。
    • 非阻塞在滿時回 false，並記錄 dropped 計數（限流可觀測）。
    • 記憶體序：Producer release → Consumer acquire；Consumer release → Producer acquire。
    • 若需求是保留最新資料，可改成 drop-old（覆蓋最舊）策略。
    ───────────────────────────────────────────────────────────
    */
    return 0;
}
