#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>

// =================================================================
// 範例：std::atomic<int> 的實際操作
// 目的：展示在多執行緒環境下，atomic 和 non-atomic 變數的差別
// =================================================================

// --- 全域變數 ---

// 1. 普通的、非執行緒安全的整數計數器
int g_normal_counter = 0;

// 2. 使用 std::atomic 包裹的、執行緒安全的整數計數器
std::atomic<int> g_atomic_counter{0}; // 使用 C++11 風格的 {} 初始化


// --- 執行緒要執行的工作函式 ---

void increment_counters(int num_increments) {
    for (int i = 0; i < num_increments; ++i) {
        // 對普通整數進行 ++ 操作 (非原子，有競爭條件！)
        g_normal_counter++;

        // 對原子整數進行 ++ 操作 (原子操作，執行緒安全)
        // 底層會呼叫 fetch_add()
        g_atomic_counter++;
    }
}

// --- 主函式 ---

int main() {
    const int num_threads = 10;
    const int increments_per_thread = 100000;

    std::cout << "--- Demonstrating std::atomic<int> ---" << std::endl;
    std::cout << "Launching " << num_threads << " threads, each incrementing " 
              << increments_per_thread << " times." << std::endl;

    // 建立一個 vector 來存放我們的執行緒
    std::vector<std::thread> threads;

    // 啟動所有執行緒
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(increment_counters, increments_per_thread);
    }

    // 等待所有執行緒都執行完畢
    for (auto& t : threads) {
        t.join();
    }

    // 計算理論上的正確值
    const int expected_value = num_threads * increments_per_thread;

    // 輸出最終結果
    std::cout << "\n--- Results ---" << std::endl;
    std::cout << "Expected value:         " << expected_value << std::endl;
    std::cout << "Final Normal Counter:   " << g_normal_counter << " (Incorrect due to race conditions!)" << std::endl;
    
    // 讀取 atomic 變數的值，推薦使用 .load()
    std::cout << "Final Atomic Counter:   " << g_atomic_counter.load() << " (Correct!)" << std::endl;
    
    std::cout << "\n--- Test Ended ---" << std::endl;

    return 0;
}