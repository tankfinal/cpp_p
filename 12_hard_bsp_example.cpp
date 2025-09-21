#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <atomic>
#include <thread>
#include <chrono>
#include <iostream>

/*
──────────────────────────────────────────────────────────────────────────────
[面試前備忘錄｜Google Embedded 視角]
• 目標：以 BSP 封裝硬體差異，上層 App 只用 BSP API → 可攜性與可測試性。
• UART：非阻塞 → 用 SPSC ring buffer，由 ISR/背景工作者送資料；避免在 ISR 阻塞或 busy-wait。
• TIMER：提供「ticks/now/sleep_until」；轉換請用 64-bit 避免溢位。
• 實機要點：MMIO + volatile、RMW 危險（W1C/SET/CLR）、必要時 memory barrier (DMB/DSB)。
• 測試：PC 上用 Mock（stdout + steady_clock）即可測行為，不碰硬體。
──────────────────────────────────────────────────────────────────────────────
*/

namespace BSP {

// ─────────────────────────────────────────────────────────────
// 介面定義：App 只看得到這些函式（或以 vtable 結構輸出亦可）
// ─────────────────────────────────────────────────────────────
void uart_init(unsigned baud);
bool uart_write(const char* s, std::size_t n); // 非阻塞：盡量送，多退回 false 表示緩衝滿
void uart_flush();                              // 等待 TX 清空（Mock 以 thread 模擬，實機等 TX 空）
void timer_init(uint32_t tick_hz);
uint64_t timer_ticks();                         // 64-bit ticks（避免週期太短）
void     sleep_until(uint64_t target_ticks);    // 忙等/事件等候（Mock 以 sleep_until 實作）

// 可選：提供「替換實作」的選項
enum class Impl { MockPC /*, RealHW*/ };
void select(Impl i);

} // namespace BSP

// ─────────────────────────────────────────────────────────────
// SPSC Ring（無鎖、2^N 容量）：供 UART TX 緩衝
// ─────────────────────────────────────────────────────────────
template <typename T, std::size_t N>
struct Ring {
    static_assert((N & (N - 1)) == 0, "N must be power of two");
    static constexpr std::size_t MASK = N - 1;
    T buf[N];
    std::atomic<std::size_t> head{0}; // producer（App）
    std::atomic<std::size_t> tail{0}; // consumer（ISR/背景）

    bool push(const T& x) {
        auto h = head.load(std::memory_order_relaxed);
        auto t = tail.load(std::memory_order_acquire);
        if (((h + 1) & MASK) == (t & MASK)) return false; // full
        buf[h & MASK] = x;
        head.store(h + 1, std::memory_order_release);
        return true;
    }
    bool pop(T& out) {
        auto t = tail.load(std::memory_order_relaxed);
        auto h = head.load(std::memory_order_acquire);
        if ((t & MASK) == (h & MASK)) return false;       // empty
        out = buf[t & MASK];
        tail.store(t + 1, std::memory_order_release);
        return true;
    }
    bool empty() const {
        return (head.load(std::memory_order_acquire) & MASK) ==
               (tail.load(std::memory_order_acquire) & MASK);
    }
};

// ─────────────────────────────────────────────────────────────
// MockPC 實作：PC 上運行（可單元測試/CI 用）
// - UART：ring buffer + 背景執行緒「模擬 ISR」逐字輸出到 stdout
// - TIMER：std::chrono::steady_clock 當作時間來源
// ─────────────────────────────────────────────────────────────
namespace MockPC {

static Ring<char, 1024> tx_ring;
static std::atomic<bool> running{false};
static std::thread tx_worker;
static uint32_t g_tick_hz = 1000; // 1kHz ticks (1ms)
static std::chrono::steady_clock::time_point t0;

// 背景執行緒：模擬 UART TX ISR（取出 ring 資料、寫 stdout）
static void tx_loop() {
    while (running.load(std::memory_order_acquire)) {
        char c;
        if (tx_ring.pop(c)) {
            std::fputc(c, stdout);
            std::fflush(stdout);
            // 模擬傳輸延遲：真實 UART 會看 baud（這裡略小延遲）
            std::this_thread::sleep_for(std::chrono::microseconds(50));
        } else {
            // 無資料 → 稍作休眠（模擬中斷觸發前的空轉）
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    }
}

void select() {
    if (running.exchange(true)) return; // 已經跑過
    t0 = std::chrono::steady_clock::now();
    tx_worker = std::thread(tx_loop);
}

void stop() {
    if (!running.exchange(false)) return;
    if (tx_worker.joinable()) tx_worker.join();
}

void uart_init(unsigned baud) {
    (void)baud; // Mock 忽略；實機會寫 MMIO 設定
}

bool uart_write(const char* s, std::size_t n) {
    // 非阻塞：能塞多少塞多少；塞不下回傳 false（讓上層決定重試/丟棄）
    bool all = true;
    for (std::size_t i = 0; i < n; ++i) {
        if (!tx_ring.push(s[i])) { all = false; break; }
    }
    return all;
}

void uart_flush() {
    // 等待送完：Mock 以輪詢 TX 空，實機會等 TX 空旗標或中斷
    while (!tx_ring.empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void timer_init(uint32_t tick_hz) {
    g_tick_hz = tick_hz ? tick_hz : 1000;
    t0 = std::chrono::steady_clock::now();
}

uint64_t timer_ticks() {
    using namespace std::chrono;
    auto now = steady_clock::now();
    auto ns  = duration_cast<nanoseconds>(now - t0).count();
    // ticks = ns / (1e9 / g_tick_hz)
    return static_cast<uint64_t>(ns / (1000000000ull / g_tick_hz));
}

void sleep_until(uint64_t target_ticks) {
    using namespace std::chrono;
    auto now_ticks = timer_ticks();
    if (target_ticks <= now_ticks) return;
    auto delta_ticks = target_ticks - now_ticks;
    auto ns = delta_ticks * (1000000000ull / g_tick_hz);
    std::this_thread::sleep_for(nanoseconds(ns));
}

} // namespace MockPC

// ─────────────────────────────────────────────────────────────
// RealHW 實機 stub（不在 PC 連結）：展示 MMIO/volatile/barrier 位置
// ─────────────────────────────────────────────────────────────
namespace RealHW {
#if 0   // 啟用須換成目標平台位址與 barrier
    using u32 = std::uint32_t;
    static constexpr std::uintptr_t UART_BASE = 0x4000'1000;
    static constexpr std::uintptr_t TIMER_BASE= 0x4000'2000;
    static constexpr u32 UART_TXDATA_OFF = 0x00;
    static constexpr u32 UART_STATUS_OFF = 0x04;
    static constexpr u32 UART_STAT_TXEMPTY = (1u << 0);
    inline volatile u32* reg(std::uintptr_t base, u32 off){
        return reinterpret_cast<volatile u32*>(base + off);
    }
    inline void dmb(){ asm volatile("dmb sy" ::: "memory"); } // ARM 例

    void select() { /* 啟動 clock gating、pinmux、enable UART IRQ... */ }

    void uart_init(unsigned baud) {
        (void)baud;
        // 設 divisor、8N1、使能 TX；參照 datasheet
    }

    bool uart_write(const char* s, std::size_t n) {
        // 非阻塞：若 TX FIFO 滿就返回 false（由上層決定再試）
        for (std::size_t i=0;i<n;i++){
            if ( (*reg(UART_BASE,UART_STATUS_OFF) & UART_STAT_TXEMPTY) == 0 )
                return false;
            *reg(UART_BASE,UART_TXDATA_OFF) = static_cast<u32>(static_cast<unsigned char>(s[i]));
            dmb();
        }
        return true;
    }

    void uart_flush() {
        // 等待 TX 空（或用中斷通知）
        while ( (*reg(UART_BASE,UART_STATUS_OFF) & UART_STAT_TXEMPTY) == 0 ) { /* spin 或低功耗等待 */ }
        dmb();
    }

    void timer_init(uint32_t tick_hz) {
        (void)tick_hz;
        // 設定 timer 分頻、啟動 free-running counter
    }

    uint64_t timer_ticks() {
        // 讀 64-bit（若硬體是 2×32 位，需讀高→低→再讀高確認一致）
        volatile u32* LOW  = reg(TIMER_BASE,0x00);
        volatile u32* HIGH = reg(TIMER_BASE,0x04);
        u32 hi1 = *HIGH; u32 lo = *LOW; u32 hi2 = *HIGH;
        if (hi1 != hi2) lo = *LOW; // 處理翻轉
        return (static_cast<uint64_t>(hi2) << 32) | lo;
    }

    void sleep_until(uint64_t target_ticks) {
        // 最簡 busy-wait；實作上可用中斷 + WFI 省電
        while (timer_ticks() < target_ticks) { /* nop */ }
    }
#endif
} // namespace RealHW

// ─────────────────────────────────────────────────────────────
// BSP 封裝選擇：把函式指標指到 Mock 或 Real
// ─────────────────────────────────────────────────────────────
namespace BSP {
namespace detail {
    using UartInit   = void(*)(unsigned);
    using UartWrite  = bool(*)(const char*, std::size_t);
    using UartFlush  = void(*)();
    using TimerInit  = void(*)(uint32_t);
    using TimerTicks = uint64_t(*)();
    using SleepUntil = void(*)(uint64_t);

    struct VTable {
        UartInit   uart_init;
        UartWrite  uart_write;
        UartFlush  uart_flush;
        TimerInit  timer_init;
        TimerTicks timer_ticks;
        SleepUntil sleep_until;
    };

    // 預設用 MockPC；實機移植時替換為 RealHW::xxx
    static VTable vt = {
        /* uart_init */  [](unsigned b){ MockPC::select(); MockPC::uart_init(b); },
        /* uart_write*/  [](const char*s,std::size_t n){ return MockPC::uart_write(s,n); },
        /* uart_flush*/  [](){ MockPC::uart_flush(); },
        /* timer_init*/  [](uint32_t hz){ MockPC::timer_init(hz); },
        /* timer_ticks*/ [](){ return MockPC::timer_ticks(); },
        /* sleep_until*/ [](uint64_t t){ MockPC::sleep_until(t); }
    };
} // namespace detail

void select(Impl i) {
    using namespace detail;
    switch (i) {
        case Impl::MockPC:
        default:
            vt = {
                [](unsigned b){ MockPC::select(); MockPC::uart_init(b); },
                [](const char*s,std::size_t n){ return MockPC::uart_write(s,n); },
                [](){ MockPC::uart_flush(); },
                [](uint32_t hz){ MockPC::timer_init(hz); },
                [](){ return MockPC::timer_ticks(); },
                [](uint64_t t){ MockPC::sleep_until(t); }
            };
            break;
        // case Impl::RealHW:  // 啟用時改綁定到 RealHW
        //     vt = {
        //         RealHW::select,
        //         RealHW::uart_init,
        //         RealHW::uart_write,
        //         RealHW::uart_flush,
        //         RealHW::timer_init,
        //         RealHW::timer_ticks,
        //         RealHW::sleep_until
        //     };
        //     break;
    }
}

void uart_init(unsigned baud)                         { detail::vt.uart_init(baud); }
bool uart_write(const char* s, std::size_t n)         { return detail::vt.uart_write(s,n); }
void uart_flush()                                     { detail::vt.uart_flush(); }
void timer_init(uint32_t tick_hz)                     { detail::vt.timer_init(tick_hz); }
uint64_t timer_ticks()                                { return detail::vt.timer_ticks(); }
void sleep_until(uint64_t target_ticks)               { detail::vt.sleep_until(target_ticks); }

} // namespace BSP

// ─────────────────────────────────────────────────────────────
// App：同一套 API，Mock/Real 可切換；示範非阻塞寫入 + flush + 定時
// ─────────────────────────────────────────────────────────────
int main() {
    // 選 Mock（PC）
    BSP::select(BSP::Impl::MockPC);

    // 初始化
    BSP::uart_init(115200);
    BSP::timer_init(1000); // 1kHz ticks

    // 非阻塞 UART：可能塞不完，自己重試或丟棄
    const char* msg = "Hello from BSP (non-blocking UART)…\n";
    while (!BSP::uart_write(msg, std::char_traits<char>::length(msg))) {
        // 緩衝滿 → 稍等再試（實機常用事件/IRQ 通知，而非輪詢）
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // 每 200ms 印一次 ticks 三次
    uint64_t t = BSP::timer_ticks();
    for (int i = 0; i < 3; ++i) {
        t += 200; // 200ms at 1kHz
        BSP::sleep_until(t);
        char buf[64];
        std::snprintf(buf, sizeof(buf), "ticks=%llu\n",
                      static_cast<unsigned long long>(BSP::timer_ticks()));
        BSP::uart_write(buf, std::char_traits<char>::length(buf));
    }

    BSP::uart_flush(); // 等待送完（Mock 會等背景 thread 清空）

    /*
    ───────────────────────────────────────────────────────────
    [總結口條（可直接講）]
    • 我把 App 與硬體解耦：App 只用 BSP API；Mock/Real 透過 vtable 切換。
    • UART 走非阻塞：App push 到 ring，由「ISR/背景工人」送出（Mock 用 thread 模擬）。
    • TIMER 用 ticks/now/sleep_until；PC 用 steady_clock，實機換 MMIO counter。
    • 實機須處理：volatile MMIO、RMW/W1C、必要的 memory barrier、以及 IRQ 安全。
    ───────────────────────────────────────────────────────────
    */
    // 收尾：停止 Mock 背景執行緒（示範用；實際程式可由 exit 時自動終止）
    // （這裡不暴露 Mock 停止 API，是為了讓 BSP 介面更純；測試框架可提供 teardown）
    return 0;
}
