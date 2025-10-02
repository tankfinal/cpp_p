// =================================================================
// 題目一: 安全的讀取硬體計時器 (附 main 函式)
// =================================================================
#include <iostream>
#include <cstdint>
#include <thread>
#include <chrono>
#include <atomic>

/*
[核心觀念]
- 控制IRQ (Interrupt Control) 的必要性
- volatile 的作用：防止編譯器不當優化硬體相關的記憶體存取
*/

// --- 模擬的硬體與底層環境 ---
// 在真實系統中，這會是固定的記憶體位址。這裡我們用全域變數模擬。
volatile uint32_t MOCK_TIMER_LOW  = 0;
volatile uint32_t MOCK_TIMER_HIGH = 0;

// 模擬的中斷控制函式 (在 main 裡我們不會使用這個版本)
void disable_irq() { /* std::cout << "IRQ Disabled\n"; */ }
void enable_irq() { /* std::cout << "IRQ Enabled\n"; */ }
// --- 模擬環境結束 ---

// 安全讀取 64 位元計時器的函式
uint64_t read_timer() {
    uint32_t high1, low;

    // 使用「讀-讀-再讀」策略，避免了禁用中斷帶來的延遲
    do {
        // 先讀高位
        high1 = MOCK_TIMER_HIGH;
        // 再讀低位
        low = MOCK_TIMER_LOW;
        // 檢查在我們讀取 low 的期間，high 是否發生了變化 (進位)
        // 如果 high 變了，代表我們讀到了一個不一致的 (撕裂的) 值，需要重讀
    } while (high1 != MOCK_TIMER_HIGH);

    return ((uint64_t)high1 << 32) | low;
}

uint64_t read_timer_with_irq_disabled() {
    disable_irq(); // 進入臨界區，沒有人能打擾我

    uint32_t low = MOCK_TIMER_LOW;
    uint32_t high = MOCK_TIMER_HIGH;

    enable_irq(); // 離開臨界區

    return ((uint64_t)high << 32) | low;
}

// --- main 函式用於測試 ---
int main() {
    std::cout << "--- Testing Safe Hardware Timer Read ---" << std::endl;

    std::atomic<bool> stop_timer{false};

    // 1. 建立一個執行緒來模擬計時器硬體的不斷更新
    std::thread timer_thread([&]() {
        uint64_t counter = 0;
        while (!stop_timer.load()) {
            counter++;
            // 模擬更新 64 位元計時器 (非原子操作！)
            MOCK_TIMER_LOW = (uint32_t)counter;
            // 為了讓 Torn Read 更容易發生，在兩次更新間加入極短延遲
            std::this_thread::sleep_for(std::chrono::nanoseconds(1));
            MOCK_TIMER_HIGH = (uint32_t)(counter >> 32);
        }
    });

    // 2. 主執行緒持續讀取計時器
    for (int i = 0; i < 10; ++i) {
        uint64_t timer_value = read_timer();
        std::cout << "Main thread read timer value: " << timer_value << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // 3. 停止計時器執行緒並等待它結束
    stop_timer = true;
    timer_thread.join();

    std::cout << "--- Test Ended ---" << std::endl;
    return 0;
}

/*
如何編譯與執行:
g++ your_file_name.cpp -std=c++11 -o program -pthread
*/