// =================================================================
// 題目三: 設計一個可取消的阻塞任務 (附 main 函式)
// =================================================================
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <atomic>

/*
[核心觀念]
- block I/O (阻塞式 I/O) 的挑戰
- 如何cancel task (如何優雅地取消任務)
- std::condition_variable 的妙用：打破阻塞
*/

class CancellableTask {
private:
    std::mutex mtx_;
    std::condition_variable cv_;
    std::atomic<bool> is_cancelled_{false};

    // 模擬一個慢速的、可被喚醒的 I/O 操作
    void stoppable_slow_io() {
        std::unique_lock<std::mutex> lock(mtx_);
        std::cout << "[Worker] Starting slow I/O, will wait up to 10 seconds..." << std::endl;

        // cv_.wait_for: 等待一段時間，或直到被 notify() 喚醒
        // 它會檢查一個 predicate (第三個參數)，如果為 true 就直接返回，避免 spurious wakeups
        if (cv_.wait_for(lock, std::chrono::seconds(10), [this]{ return is_cancelled_.load(); })) {
            std::cout << "[Worker] Woken up because task was cancelled." << std::endl;
        } else {
            std::cout << "[Worker] I/O operation finished normally after timeout." << std::endl;
        }
    }

public:
    void run() {
        std::cout << "[Worker] Task started." << std::endl;
        stoppable_slow_io();
        if (is_cancelled_) {
             std::cout << "[Worker] Task exiting due to cancellation." << std::endl;
        } else {
             std::cout << "[Worker] Task exiting normally." << std::endl;
        }
    }

    void cancel() {
        std::cout << "[Main]   Sending cancel signal to worker..." << std::endl;
        // 1. 設定旗標
        is_cancelled_ = true;
        // 2. 喚醒正在等待的執行緒
        cv_.notify_one();
    }
};

// --- main 函式用於測試 ---
int main() {
    std::cout << "--- Testing Cancellable Blocking Task ---" << std::endl;

    CancellableTask task;

    // 1. 啟動工作執行緒，它會立刻進入阻塞狀態
    std::thread worker(&CancellableTask::run, &task);

    // 2. 主執行緒等待 2 秒
    std::cout << "[Main]   Sleeping for 2 seconds before sending cancel signal." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // 3. 從主執行緒取消工作執行緒
    task.cancel();

    // 4. 等待工作執行緒完全結束
    worker.join();

    std::cout << "--- Test Ended ---" << std::endl;
    return 0;
}

/*
如何編譯與執行:
g++ your_file_name.cpp -std=c++11 -o program -pthread
*/