#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>

// =================================================================
// 範例：std::atomic<int> 的實際操作 (附詳細註解版)
// 目的：展示在多執行緒環境下，atomic 和 non-atomic 變數的差別，
//       並解釋 std::thread 的建立與管理。
// =================================================================

// --- 全域變數 ---

// 1. 普通的、非執行緒安全的整數計數器
int g_normal_counter = 0;

// 2. 使用 std::atomic 包裹的、執行緒安全的整數計數器
std::atomic<int> g_atomic_counter{0};


// --- 執行緒要執行的工作函式 ---

void increment_counters(int num_increments) {
    for (int i = 0; i < num_increments; ++i) {
        // 對普通整數進行 ++ 操作 (非原子，有競爭條件！)
        g_normal_counter++;

        // 對原子整數進行 ++ 操作 (原子操作，執行緒安全)
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

    // 建立一個 vector 來存放我們的執行緒物件。
    // 這是在 C++ 中管理一組執行緒的標準且常見的做法。
    std::vector<std::thread> threads;

    // =================================================================
    // === 觀念解析 1: 執行緒的建立與啟動 ===
    // =================================================================
    // `emplace_back(...)` 的作用是在 vector 的尾端「原地建構」一個新元素。
    // 在這裡，它會直接呼叫 `std::thread` 的建構函式來建立一個新的執行緒物件。
    //
    // `std::thread(increment_counters, increments_per_thread)`
    //  - 第一個參數 `increment_counters`：是這個新執行緒要去執行的函式。
    //  - 後面的參數 `increments_per_thread`：是要傳遞給該函式的參數。
    //
    // 關鍵點 1: 每一次迴圈，只會建立「一個」新的執行緒。
    //
    // 關鍵點 2: `std::thread` 在它被成功建構的那一瞬間，就立刻開始並行執行了！
    //          主執行緒 (main) 不會等待，會馬上繼續執行 for 迴圈的下一次迭代。
    //
    // 關鍵點 3: 之所以能將 std::thread 放入 vector，是因為 std::thread 物件雖然
    //          不可複製 (non-copyable)，但它是可移動的 (movable)，而 emplace_back
    //          正好能處理這種情況。
    // =================================================================

    // 啟動所有執行緒
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(increment_counters, increments_per_thread);
    }

    // 當上面的 for 迴圈結束時，我們已經有 10 個執行緒在背景「同時」執行了。

    // =================================================================
    // === 觀念解析 2: 等待執行緒結束 (join) ===
    // =================================================================
    // `join()` 的意思是「加入」，它是一個阻塞 (blocking) 的呼叫。
    // 主執行緒執行到 `t.join()` 時，會停在這裡，一直「等待」 `t` 這個子執行緒
    // 完全執行完畢，然後主執行緒才能繼續往下走。
    //
    // 為什麼必須 `join`？
    // 如果沒有這個迴圈，`main` 函式會立刻往下衝，去印出結果然後結束。
    // 這會導致兩個問題：
    // 1. 結果不對：子執行緒們根本還沒跑完，`g_normal_counter` 和 `g_atomic_counter`
    //             的值會遠小於預期值。
    // 2. 程式崩潰：當 `main` 函式結束時，如果還有子執行緒在背景執行，
    //             C++ 執行期會呼叫 `std::terminate` 強制終止整個程式。
    //
    // 透過迴圈對每一個執行緒呼叫 `join()`，我們確保了主執行緒會等到
    // 所有 100 萬次累加都確實完成後，才去印出最終結果。
    // =================================================================

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