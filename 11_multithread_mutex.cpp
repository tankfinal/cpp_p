#include <iostream>
#include <thread>
#include <mutex>
#include <functional>

/*
──────────────────────────────────────────────────────────────────────────────
[面試前備忘錄｜Google Embedded 視角]
• mutex = 互斥鎖，用於保護共享資源，避免 data race。
• std::lock_guard = RAII 包裝：建構時 lock，解構時 unlock → 避免忘記 unlock。
• 此範例：兩個 thread 同時對 counter++，若無鎖會 race condition；用 mutex 保證正確。

• Embedded 特殊角度：
  - 小型 MCU 常無「thread」，但在 RTOS (FreeRTOS/Zephyr) 有 task + mutex/semaphore。
  - 在 ISR 中不能用 mutex → 要用 atomic 或 lock-free 資料結構。
  - 多核 SoC：mutex 會觸發 cache coherence 流量（效能考量）。

──────────────────────────────────────────────────────────────────────────────
[常見追問（口條提示）]
Q1: 為什麼要用 std::lock_guard 而不是手動 lock/unlock？
A1: lock_guard 是 RAII，例外安全，不會忘記 unlock（避免死鎖）。

Q2: mutex vs atomic<int>？
A2: atomic<int> 可解決簡單計數器；mutex 適合複雜臨界區或多資源同步。

Q3: 什麼是死鎖？如何避免？
A3: 多鎖交叉等待導致僵局；避免辦法：固定加鎖順序、lock() + std::adopt_lock、或使用 lock-free。

Q4: Embedded 如果沒有 STL，怎麼辦？
A4: 用 RTOS API（例如 FreeRTOS xSemaphoreTake/Give），或用關中斷保護臨界區。

──────────────────────────────────────────────────────────────────────────────
[陷阱備忘]
• 忘記 join → 主程式結束 thread 還沒收回 → crash/未定義行為。
• mutex 保護範圍過大會降低效能（granularity 適中）。
• 不要在 ISR 裡鎖 mutex（會死鎖或延遲不可接受）。
• lock_guard 不能解鎖後再重入 → 若需要可重入要用 recursive_mutex。
──────────────────────────────────────────────────────────────────────────────
*/

std::mutex m;
int counter = 0;

// worker：每個 thread 執行多次 ++counter
void worker(int id, int times) {
    for (int i = 0; i < times; i++) {
        std::lock_guard<std::mutex> lk(m); // RAII：lock + unlock 自動管理
        ++counter;
    }
}

int main() {
    // std::bind 將 worker + 參數綁成 callable → 丟給 thread 建構子
    std::thread t1(std::bind(worker, 1, 100000));
    std::thread t2(std::bind(worker, 2, 100000));

    // join 等待 thread 完成，避免主程式先退出
    t1.join();
    t2.join();

    std::cout << "counter=" << counter << "\n"; // 正確結果：200000

    /*
    ───────────────────────────────────────────────────────────
    [總結口條（可直接講）]
    • 這是 mutex + lock_guard 確保 counter++ 正確的範例。
    • 若不用鎖，會 data race → 結果小於 200000。
    • atomic<int> 也能解這個問題，但 mutex 更通用。
    • 在 embedded RTOS 會用 semaphore/mutex API；ISR 內則需用 atomic 或 disable IRQ。
    ───────────────────────────────────────────────────────────
    */

    return 0;
}
