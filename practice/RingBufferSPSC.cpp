#include <atomic>
#include <cstdint>
#include <array>

// TASK: Implement SPSC ring buffer for uint8_t with size 256 (power-of-two).
// Provide push/pop/empty/full; use atomic indices and proper memory order.

struct Ring {
    static const uint16_t N = 256;                 // must be power of two
    std::array<uint8_t, N> buf;                    // no {} init (C++11-safe)
    std::atomic<uint16_t> head;                    // next write index
    std::atomic<uint16_t> tail;                    // next read index

    Ring() : head(0), tail(0) {}                   // C++11 ctor init

    bool push(uint8_t v) {
        (void)v;
        // TODO: implement single-producer push
        return false;
    }

    bool pop(uint8_t& out) {
        (void)out;
        // TODO: implement single-consumer pop
        return false;
    }

    bool empty() const {
        // TODO: check if ring is empty
        return true;
    }

    bool full() const {
        // TODO: check if ring is full
        return false;
    }
};

int main() {
    Ring r;           // skeleton should link/run
    (void)r;          // suppress unused warning
    return 0;
}

//
// #include <atomic>
// #include <cstdint>
// #include <array>
// #include <thread>
// #include <chrono>
// #include <iostream>
//
// /*
// [面試要點]
// • 情境：ISR 產生資料（producer），主迴圈取資料（consumer）。
// • SPSC ring：單生產者寫 head、單消費者寫 tail → 無需鎖，O(1)。
// • 記憶體序：producer 寫資料後以 release 發佈 head；consumer 以 acquire 觀察 head 後再讀資料。
// • power-of-two 容量：用 &(N-1) 取代 %，避免除法。
// */
//
// struct Ring {
//     static const uint16_t N = 256;                 // power-of-two
//     std::array<uint8_t, N> buf{};
//     std::atomic<uint16_t> head{0};                 // producer: next write
//     std::atomic<uint16_t> tail{0};                 // consumer: next read
//
//     bool push(uint8_t v) {                         // ISR/producer 呼叫
//         uint16_t h = head.load(std::memory_order_relaxed);
//         uint16_t t = tail.load(std::memory_order_acquire);
//         if (((h + 1) & (N - 1)) == (t & (N - 1))) return false; // full
//         buf[h & (N - 1)] = v;                      // 先寫資料
//         head.store(h + 1, std::memory_order_release); // 再發佈
//         return true;
//     }
//
//     bool pop(uint8_t& out) {                       // 主迴圈/consumer 呼叫
//         uint16_t t = tail.load(std::memory_order_relaxed);
//         uint16_t h = head.load(std::memory_order_acquire);
//         if ((t & (N - 1)) == (h & (N - 1))) return false;       // empty
//         out = buf[t & (N - 1)];
//         tail.store(t + 1, std::memory_order_release);
//         return true;
//     }
//     bool empty() const {
//         return head.load(std::memory_order_acquire) ==
//                tail.load(std::memory_order_acquire);
//     }
// };
//
// int main() {
//     Ring r;
//     std::atomic<bool> running{true};
//
//     // 模擬「ISR」：固定週期推資料（真實情境由中斷觸發）
//     std::thread isr([&]{
//         uint8_t seq = 1;
//         using namespace std::chrono;
//         while (running.load(std::memory_order_relaxed)) {
//             if (!r.push(seq)) {
//                 // 緩衝滿 → 模擬丟包或計數
//                 // 在真實系統可記錄 overflow 計數器
//             } else {
//                 seq++;
//             }
//             std::this_thread::sleep_for(milliseconds(5)); // 模擬取樣/到貨頻率
//         }
//     });
//
//     // 主迴圈：以較慢頻率取資料做處理
//     using namespace std::chrono;
//     auto t0 = steady_clock::now();
//     while (steady_clock::now() - t0 < seconds(1)) { // 跑 1 秒
//         uint8_t v;
//         int drained = 0;
//         while (r.pop(v)) {
//             // 處理資料（在 embedded 可能是解碼/封包/寫 DMA）
//             std::cout << "got " << static_cast<int>(v) << "\n";
//             drained++;
//         }
//
//         // 沒資料時可睡一下（避免忙等）；RT 系統可用事件/旗號喚醒
//         if (drained == 0) std::this_thread::sleep_for(milliseconds(1));
//     }
//
//     running.store(false, std::memory_order_relaxed);
//     isr.join();
//
//     /*
//     [總結口條]
//     • ISR 只做「輕量 push 到 ring」；主迴圈再批次處理 → 簡潔、避免 ISR 長時間佔用。
//     • 空/滿管理：保留一格；滿時策略是丟包或回報 overflow。
//     • 若搬到 MCU：把 thread 換成真正的中斷服務常式；邏輯相同。
//     */
//     return 0;
// }
