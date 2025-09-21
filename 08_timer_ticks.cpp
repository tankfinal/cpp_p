#include <iostream>
#include <cstdint>

/*
──────────────────────────────────────────────────────────────────────────────
[面試前備忘錄｜Google Embedded 視角]
• Timer 計數器 (tick counter)：MCU 常見硬體，固定 tick period 遞增。
• tick <-> ns 轉換要用 64 位中間值，避免 32 位溢位。
• wraparound：32 位無號整數溢位後會自動 mod 2^32，
  → C/C++ 無號減法是 well-defined，可用 now - start 直接算經過 ticks。
• 嵌入式常見用途：
  - 計算 ISR 執行時間、timeout 判斷、scheduler tick。
  - busy-wait loop 或 delay()。
──────────────────────────────────────────────────────────────────────────────
[常見追問（你可口述）]
Q1: 為什麼要用 uint64_t 中間值？
A1: 若用 32 位，ticks * period 可能溢位；64 位保證安全。

Q2: 為什麼 wraparound 可以用 unsigned 減法解？
A2: 無號整數在 C/C++ 定義為 mod 2^N，差值仍正確（直到超過完整週期）。

Q3: 若 tick counter 不是 32 位，而是 16 位呢？
A3: 同理，差值在 0..65535 範圍內正確，週期內計算都安全。

Q4: 怎麼避免 busy-wait 浪費 CPU？
A4: 用 timer 中斷 + ISR 或 RTOS delay API，比輪詢省電且精確。

──────────────────────────────────────────────────────────────────────────────
[陷阱備忘]
• 千萬不要用有號 int 計時，因為溢位是 UB（未定義行為）。
• tick period 不是 ns 而是 CPU/Timer clock → 要根據時脈換算。
• elapsed_ticks 只能保證差值 < 2^32；超過一圈就模糊，應在合理時間窗內使用。
──────────────────────────────────────────────────────────────────────────────
*/

// 將 ticks 轉成奈秒（ns）
uint64_t ticks_to_ns(uint32_t ticks, uint32_t tick_period_ns) {
    // 64 位乘法避免 32 位溢位
    return static_cast<uint64_t>(ticks) * static_cast<uint64_t>(tick_period_ns);
}

// 將奈秒（ns）轉回 ticks（整除取下界）
uint32_t ns_to_ticks(uint64_t ns, uint32_t tick_period_ns) {
    return static_cast<uint32_t>(ns / tick_period_ns);
}

// 計算經過的 ticks（處理 32 位無號溢位）
inline uint32_t elapsed_ticks(uint32_t start, uint32_t now) {
    // 無號減法在 C/C++ 定義為 mod 2^32，天然處理 wraparound
    return now - start;
}

int main() {
    // ── 範例設定 ───────────────────────────────
    // 1 tick = 10us = 10,000ns
    uint32_t tick_ns = 10000;

    // ticks -> ns
    uint32_t t = 12345;
    std::cout << "ticks->ns: " << ticks_to_ns(t, tick_ns) << " ns\n";

    // ns -> ticks
    uint64_t want_ns = 5000000; // 5ms
    std::cout << "ns->ticks: " << ns_to_ticks(want_ns, tick_ns) << " ticks\n";

    // wraparound 範例
    uint32_t start = 0xFFFF0000u;
    uint32_t now   = 0x00001000u;
    std::cout << "elapsed_ticks (with wrap): "
              << elapsed_ticks(start, now) << " ticks\n";

    /*
    ───────────────────────────────────────────────────────────
    [總結口條（可直接講）]
    • 我用 uint64_t 避免 tick->ns 時溢位。
    • elapsed_ticks 用無號減法，天然處理 wraparound。
    • 若在 embedded，tick 週期長度決定了最大可量測時間窗。
    • 如果要低功耗，我會用 timer interrupt，而不是 busy loop。
    ───────────────────────────────────────────────────────────
    */
    return 0;
}
