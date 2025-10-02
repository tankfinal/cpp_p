// =================================================================
// 題目一: 中斷安全的環形緩衝區 (Interrupt-Safe Ring Buffer)
// Google L6 Embedded Cloud Storage 面試題
// =================================================================

#include <iostream>
#include <vector>
#include <atomic>      // 為了 std::atomic，實現無鎖操作的關鍵
#include <optional>    // 為了 std::optional，用於回傳可能為空的讀取結果 (需要 C++17)
#include <thread>      // 為了 std::thread，用於模擬併發場景
#include <chrono>      // 為了 std::chrono，用於執行緒休眠
#include <mutex>       // 為了 std::mutex，用於保護 std::cout 的輸出，避免錯亂

/*
[題目描述]
請設計並實作一個高效能的、執行緒安全的環形緩衝區 (Ring Buffer 或 Circular Buffer)，
用於在一個嵌入式系統中處理高頻數據。

具體場景與要求：
1.  生產者 (Producer)：一個中斷服務常式 (ISR / IRQ Handler)，負責從硬體（例如：ADC 或網路卡）
    高速接收數據封包，並將其寫入環形緩衝區。
2.  消費者 (Consumer)：一個在背景執行的低優先權執行緒，負責從環形緩-衝區中讀取數據封包，
    並進行後續處理（例如：上傳到雲端）。
3.  執行緒安全：必須保證 ISR 和背景執行緒之間的無鎖操作 (lock-free)，以避免在 ISR 中
    使用 `mutex` 造成的死鎖或優先權反轉風險。
4.  記憶體效率：緩衝區的大小在初始化時固定，不應有任何動態記憶體配置 (`new`/`delete`)。
5.  關鍵字考察：你的設計需要解釋 `volatile` 在此場景下的局限性，以及為何現代 C++
    應優先使用 `std::atomic`。

[思路與設計方向]
1.  資料結構：
    - 底層使用一個固定大小的 `std::vector` 作為儲存介質。
    - 需要兩個索引 `head` 和 `tail` 來分別管理讀取和寫入的位置。

2.  併發模型 (Concurrency Model)：
    - 採用经典的「單生產者-單消費者」(SPSC - Single-Producer, Single-Consumer) 模型。
    - 生產者 (ISR) 是唯一的寫入者，只會推進 `tail` 指標。
    - 消費者 (背景執行緒) 是唯一的讀取者，只會推進 `head` 指標。
    - 由於讀寫操作分離，為實現無鎖 (lock-free) 設計提供了基礎。

3.  無鎖的實現 (Lock-Free Implementation)：
    - `head` 和 `tail` 索引必須宣告為 `std::atomic` 型別。
    - `std::atomic` 能保證對索引的讀取和寫入操作是「原子」的，不會被中斷，從而避免了
      讀到更新一半的數值。
    - 同時，`std::atomic` 提供了必要的「記憶體順序」(Memory Ordering) 保證，確保一個
      CPU 核心 (ISR) 對記憶體的寫入，能夠被另一個 CPU 核心 (背景執行緒) 正確地看到。
      這是 `volatile` 無法提供的關鍵保證。

4.  邊界條件處理：
    - 為了區分「緩衝區已滿」和「緩衝區為空」這兩種 `head == tail` 的情況，
      我們通常會讓緩衝區的實際大小比宣告的容量多 1。
    - 緩衝區為空 (Empty): `head == tail`
    - 緩衝區已滿 (Full): `(tail + 1) % size == head`
*/

// --- 解答 (Solution) ---

template<typename T, size_t Capacity>
class InterruptSafeRingBuffer {
private:
    // 預分配固定大小的緩衝區，大小為 Capacity + 1 以便區分滿/空狀態
    std::vector<T> buffer_;

    // 使用 std::atomic 來保證索引操作的原子性和跨執行緒的可見性
    std::atomic<size_t> head_; // 由消費者 (讀取者) 修改
    std::atomic<size_t> tail_; // 由生產者 (寫入者) 修改

public:
    // 建構函式
    InterruptSafeRingBuffer() : buffer_(Capacity + 1), head_(0), tail_(0) {}

    // 禁止複製和賦值，因為這類資源管理類別通常不應該被複製
    InterruptSafeRingBuffer(const InterruptSafeRingBuffer&) = delete;
    InterruptSafeRingBuffer& operator=(const InterruptSafeRingBuffer&) = delete;

    // 寫入操作 (由生產者/ISR呼叫)
    // 使用 const T& 來接收，避免不必要的複製
    bool write(const T& item) {
        const size_t current_tail = tail_.load(std::memory_order_relaxed);
        const size_t next_tail = (current_tail + 1) % buffer_.size();

        // 使用 acquire memory order 來讀取 head，確保能看到消費者執行緒對 head 的最新修改
        if (next_tail == head_.load(std::memory_order_acquire)) {
            return false; // 緩衝區已滿
        }

        buffer_[current_tail] = item;

        // 使用 release memory order 來更新 tail，確保對 buffer 的寫入操作
        // 對於之後讀取 tail 的消費者執行緒是可見的
        tail_.store(next_tail, std::memory_order_release);
        return true;
    }

    // 讀取操作 (由消費者/背景執行緒呼叫)
    std::optional<T> read() {
        const size_t current_head = head_.load(std::memory_order_relaxed);

        // 使用 acquire memory order 來讀取 tail，確保能看到生產者執行緒對 buffer 和 tail 的最新修改
        if (current_head == tail_.load(std::memory_order_acquire)) {
            return std::nullopt; // 緩衝區為空
        }

        // 從緩衝區讀取數據
        std::optional<T> item = std::move(buffer_[current_head]);

        // 使用 release memory order 來更新 head，確保其他執行緒（如果有的話，雖然SPSC模型沒有）
        // 能看到我們對 head 的修改
        head_.store((current_head + 1) % buffer_.size(), std::memory_order_release);
        return item;
    }
};

// --- main 函式用於測試 ---

// 為了讓多執行緒的 cout 輸出不錯亂，我們用一個全域 mutex 來保護它
std::mutex cout_mutex;

int main() {
    std::cout << "--- Testing Interrupt-Safe Ring Buffer (C++17) ---" << std::endl;

    // 建立一個容量為 8 的環形緩衝區來儲存 int
    InterruptSafeRingBuffer<int, 8> buffer;

    // 標誌，用於通知消費者執行緒可以結束了
    std::atomic<bool> producer_done{false};

    // 1. 生產者執行緒 (模擬高頻的 ISR)
    // 使用 lambda 函式來定義執行緒的執行內容
    std::thread producer([&]() {
        for (int i = 0; i < 20; ++i) {
            // 不斷嘗試寫入，直到成功
            while (!buffer.write(i)) {
                // 在真實 ISR 中不應有 sleep，這裡只是為了模擬，避免 CPU 滿載
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }

            // 使用 lock_guard 保護 cout，確保輸出不會被打斷
            {
                std::lock_guard<std::mutex> lock(cout_mutex);
                std::cout << "Producer: Wrote " << i << std::endl;
            }

            // 模擬 ISR 之間有短暫間隔
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
        producer_done = true; // 通知消費者，我生產完了
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "Producer: Finished." << std::endl;
        }
    });

    // 2. 消費者執行緒 (模擬背景上傳任務)
    std::thread consumer([&]() {
        // 當生產者還在生產，或緩衝區裡還有東西時，就繼續消費
        while (!producer_done || buffer.read().has_value()) {
            auto item = buffer.read();
            if (item) {
                std::lock_guard<std::mutex> lock(cout_mutex);
                std::cout << "\t\t\t\tConsumer: Read " << *item << std::endl;
            }

            // 模擬消費者處理速度比生產者慢，這樣才能測試到緩衝區滿的情況
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "\t\t\t\tConsumer: Finished." << std::endl;
        }
    });

    // 主執行緒等待兩個子執行緒都執行完畢
    producer.join();
    consumer.join();

    std::cout << "--- Test Ended ---" << std::endl;

    return 0;
}