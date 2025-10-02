// =================================================================
// 題目六: 實作一個限速器 (Rate Limiter)
// =================================================================
#include <iostream>
#include <deque>
#include <chrono>
#include <mutex>
#include <thread>
#include <string>

/*
[題目描述]
設計一個 RateLimiter 類別，它有一個方法 `should_allow(request_id)`。在過去的一段時間內（例如 1 秒），如果收到的請求總數超過 N 個，則 `should_allow` 應回傳 false，否則回傳 true。這個類別必須是執行緒安全的。

[思路]
這個問題可以使用「滑動窗口日誌 (Sliding Window Log)」演算法來解決。
1.  **資料結構**: 使用一個 `std::deque` (雙端佇列) 來儲存每個請求到達的時間戳。Deque 提供了在兩端進行 O(1) 插入和刪除的操作。
2.  **時間戳**: 使用 `std::chrono::steady_clock` 來獲取時間戳，它是一個單調時鐘，不受系統時間變化的影響，非常適合用來測量時間間隔。
3.  **`should_allow` 操作**:
    a. 首先，獲取當前時間 `now`。
    b. **滑動窗口**: 從 deque 的前端開始，移除所有過期的時間戳（即 `timestamp <= now - window_size`）。
    c. **檢查容量**: 移除過期時間戳後，檢查 deque 的大小。如果大小小於速率限制 N，表示允許該請求。
    d. **記錄請求**: 如果允許，將當前時間戳 `now` 加入 deque 的後端，並回傳 true。否則，回傳 false。
4.  **執行緒安全**: 所有對 deque 的讀寫操作都必須被一個 `std::mutex` 保護，以確保在多執行緒環境下的資料一致性。`std::lock_guard` 是一個方便的 RAII 工具，可以在建構時鎖定 mutex，並在解構時（離開作用域時）自動解鎖。
*/

// 解答 (Solution)
class RateLimiter {
private:
    const int max_requests;
    const std::chrono::seconds window_size;
    std::deque<std::chrono::steady_clock::time_point> requests;
    std::mutex mtx;

public:
    RateLimiter(int n, int seconds = 1) : max_requests(n), window_size(seconds) {}

    bool should_allow(const std::string& request_id) {
        std::lock_guard<std::mutex> lock(mtx);

        auto now = std::chrono::steady_clock::now();

        // 先把過期的彈掉
        while (!requests.empty() && requests.front() <= now - window_size) {
            requests.pop_front();
        }

        // 檢查size是否夠
        if (requests.size() < max_requests) {
            requests.push_back(now);
            std::cout << "Request " << request_id << " allowed. Window count: " << requests.size() << std::endl;
            return true;
        } else {
            std::cout << "Request " << request_id << " denied. Window count: " << requests.size() << std::endl;
            return false;
        }
    }
};

// main 函式用於測試
int main() {
    std::cout << "--- Testing Rate Limiter ---" << std::endl;
    // 1 秒內最多 3 個請求
    RateLimiter limiter(3, 1);

    std::cout << "--- Burst 1 ---" << std::endl;
    for (int i = 1; i <= 5; ++i) {
        limiter.should_allow(std::to_string(i));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "\nWaiting for 1 second for the window to slide...\n" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "--- Burst 2 ---" << std::endl;
    for (int i = 6; i <= 10; ++i) {
        limiter.should_allow(std::to_string(i));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "--- Test Ended ---" << std::endl;
    return 0;
}