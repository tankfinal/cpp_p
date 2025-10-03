#include <cstdint>
using namespace std;
// TASK: Implement safe_elapsed(start, now) that returns elapsed 32-bit ticks
// even if 'now' wrapped around (unsigned arithmetic). Add ns<->ticks helpers.
// uint32_t safe_elapsed(uint32_t start, uint32_t now){
//     // TODO: implement using unsigned wraparound property
//     return 0;
// }
// int main(){
//     // Write a few asserts
//     return 0;
// }


#include <cstdint>
#include <cassert>
#include <iostream>

/*
──────────────────────────────────────────────────────────────
[面試前備忘錄｜Google Embedded 視角]
• 32-bit timer/counter 常見於 MCU（例如 SysTick）。
• 無號運算在 C/C++ 標準下 = 模 2^N → 可天然處理 wraparound。
  safe_elapsed = (now - start)，不用額外 if。
• tick <-> ns 轉換：避免 32-bit 溢位 → 用 64-bit 中間值。
──────────────────────────────────────────────────────────────
*/

// 安全計算 elapsed ticks，處理 wraparound
inline uint32_t safe_elapsed(uint32_t start, uint32_t now) {
    return now - start;  // unsigned 減法自動模 2^32
}

// tick → ns
inline uint64_t ticks_to_ns(uint32_t ticks, uint32_t tick_period_ns) {
    return static_cast<uint64_t>(ticks) * tick_period_ns;
}

// ns → tick
inline uint32_t ns_to_ticks(uint64_t ns, uint32_t tick_period_ns) {
    return static_cast<uint32_t>(ns / tick_period_ns);
}

int main() {
    // 基本測試：無 wrap
    {
        uint32_t start = 100;
        uint32_t now   = 150;
        assert(safe_elapsed(start, now) == 50);
    }

    // 測試 wraparound：0xFFFF'FF00 → 0x00000050
    {
        uint32_t start = 0xFFFFFF00u;
        uint32_t now   = 0x00000050u;
        uint32_t e = safe_elapsed(start, now);
        std::cout << "wrap elapsed = " << e << "\n";
        assert(e == 0x150); // 0x50 + 0x100 = 336
    }

    // tick <-> ns
    {
        uint32_t tick_ns = 10000; // 1 tick = 10us
        uint32_t t = 1234;
        uint64_t ns = ticks_to_ns(t, tick_ns);
        assert(ns == 1234ull * 10000ull);

        uint64_t want_ns = 5'000'000; // 5ms
        uint32_t ticks = ns_to_ticks(want_ns, tick_ns);
        assert(ticks == 500);
    }

    std::cout << "All tests passed!\n";
    return 0;
}

/*
──────────────────────────────────────────────────────────────
[總結口條]
• 我用 unsigned wraparound：safe_elapsed = now - start，自動模 2^32。
• ticks<->ns：用 64-bit 避免溢位，轉換正確。
• 這個技巧廣泛用在 MCU timer、OS jiffies，避免額外 if 判斷。
──────────────────────────────────────────────────────────────
*/
