#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <queue>

// =================================================================
// 範例：使用 Semaphore 實現有界緩衝區 (Producer-Consumer) [附詳細註解]
// =================================================================

// --- 1. 使用 Mutex 和 Condition Variable 實現一個計數號誌 ---
// Semaphore 是一個計數器，用於控制對有限資源的存取。
class CountingSemaphore {
private:
    int count_;
    std::mutex mtx_;
    std::condition_variable cv_;

public:
    // 初始化號誌的計數
    CountingSemaphore(int initial_count) : count_(initial_count) {}

    // P() 操作 / wait() / acquire()
    // 嘗試獲取一個資源。如果資源數為 0，則呼叫者執行緒會進入睡眠等待。
    void wait() {
        std::unique_lock<std::mutex> lock(mtx_);
        // cv_.wait 會檢查條件。
        // 如果條件 (count_ > 0) 不滿足，它會原子地：
        // 1. 解開 lock
        // 2. 讓執行緒進入睡眠
        // 當被 notify 喚醒時，它會：
        // 1. 重新鎖上 lock
        // 2. 再次檢查條件，以防止「偽喚醒」
        cv_.wait(lock, [this]{ return count_ > 0; });
        // 成功獲取資源，計數減 1
        count_--;
    }

    // V() 操作 / post() / release()
    // 釋放一個資源，計數加 1，並喚醒一個可能正在等待的執行緒。
    void post() {
        std::unique_lock<std::mutex> lock(mtx_);
        // 資源計數加 1
        count_++;
        // 喚醒一個（如果有的話）正在 wait() 中睡眠的執行緒。
        cv_.notify_one();
    }
};


// --- 2. 使用 Semaphore 實現的有界緩衝區 ---
// 這個類別協調了生產者和消費者之間的互動。
template<typename T>
class BoundedBuffer {
private:
    std::queue<T> buffer_;           // 底層的資料容器，非執行緒安全
    std::mutex mtx_;                 // 一個互斥鎖，只用來保護對 buffer_ 本身的「直接操作」(push/pop)
    CountingSemaphore empty_slots_;  // 號誌：計數緩衝區中還有多少個「空格」。生產者關心。
    CountingSemaphore filled_slots_; // 號誌：計數緩衝區中已經有多少個「滿格」。消費者關心。

public:
    // 建構函式：初始化兩個號誌的計數
    BoundedBuffer(size_t size)
        : empty_slots_(size),   // 一開始，所有格子都是空的
          filled_slots_(0) {}    // 一開始，沒有任何格子是滿的

    void produce(const T& item) {
        // Step 1: 申請一個「空格」。
        // 如果 empty_slots_ 的計數 > 0，計數減 1，執行緒繼續。
        // 如果 empty_slots_ 的計數 == 0 (緩衝區已滿)，執行緒會在此處阻塞（睡眠）。
        empty_slots_.wait();

        // Step 2: 獲取互斥鎖，對共享的 queue 進行寫入。
        // 臨界區應該盡可能短。
        {
            std::lock_guard<std::mutex> lock(mtx_);
            buffer_.push(item);
        }

        // Step 3: 釋放一個「滿格」。
        // 將 filled_slots_ 的計數加 1，並可能喚醒一個正在等待的消費者。
        filled_slots_.post();
    }

    T consume() {
        // Step 1: 申請一個「滿格」。
        // 如果 filled_slots_ 的計數 > 0，計數減 1，執行緒繼續。
        // 如果 filled_slots_ 的計數 == 0 (緩衝區是空的)，執行緒會在此處阻塞（睡眠）。
        filled_slots_.wait();

        T item;
        // Step 2: 獲取互斥鎖，對共享的 queue 進行讀取。
        {
            std::lock_guard<std::mutex> lock(mtx_);
            item = buffer_.front();
            buffer_.pop();
        }

        // Step 3: 釋放一個「空格」。
        // 將 empty_slots_ 的計數加 1，並可能喚醒一個正在等待的生產者。
        empty_slots_.post();

        return item;
    }
};


// --- 3. main 函式用於測試 ---
int main() {
    std::cout << "--- Testing Bounded Buffer with Semaphores ---" << std::endl;
    // 建立一個大小為 5 的緩衝區。
    // 這意味著 `empty_slots_` 計數從 5 開始，`filled_slots_` 計數從 0 開始。
    BoundedBuffer<int> buffer(5);

    // 建立一個生產者執行緒
    std::thread producer([&]() {
        for (int i = 0; i < 10; ++i) {
            std::cout << "Producer: Producing item " << i << "..." << std::endl;
            buffer.produce(i);
            std::cout << "Producer: Successfully produced item " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });

    // 建立一個消費者執行緒
    std::thread consumer([&]() {
        for (int i = 0; i < 10; ++i) {
            // 故意讓消費速度比生產慢，這樣我們就能觀察到緩衝區被填滿，
            // 以及生產者被阻塞的情況。
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            std::cout << "\t\tConsumer: Waiting to consume..." << std::endl;
            int item = buffer.consume();
            std::cout << "\t\tConsumer: Consumed item " << item << std::endl;
        }
    });

    // 等待兩個執行緒都完成它們的工作
    producer.join();
    consumer.join();

    std::cout << "--- Test Ended ---" << std::endl;
    return 0;
}