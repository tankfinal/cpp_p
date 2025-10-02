// =================================================================
// 題目七: 設計一個分散式 ID 產生器 (Snowflake)
// =================================================================
#include <iostream>
#include <cstdint>
#include <chrono>
#include <mutex>
#include <thread>
#include <stdexcept>
#include <vector>
#include <set>

/*
[題目描述]
在一個大型分散式系統中，需要一個服務來產生獨一無二的 64-bit ID。這個 ID 應該大致上是按時間排序的。設計一個能在一台機器上運行的 ID 產生器類別。

[思路]
這是 Twitter 的 Snowflake 演算法的實作。一個 64-bit ID 可以被劃分為幾個部分：
1.  **符號位 (1 bit)**: 最高位，固定為 0，確保 ID 為正數。
2.  **時間戳 (41 bits)**: 儲存從一個「紀元時間 (epoch)」到現在的毫秒數。41 bits 可以表示約 69 年。這保證了 ID 大致上是時間遞增的。
3.  **機器 ID (10 bits)**: 預留給部署產生器的機器。10 bits 可以區分 1024 台機器，避免不同機器產生衝突。
4.  **序列號 (12 bits)**: 代表在同一毫秒內，一台機器可以產生的 ID 數量 ($2^{12} = 4096$ 個)。每產生一個 ID，序列號就加一；進入下一毫秒時，序列號重置為 0。

**執行緒安全與時鐘回撥**:
- **執行緒安全**: 對 `last_timestamp` 和 `sequence` 的讀寫操作必須是原子的，因此需要使用 `std::mutex` 來保護。
- **時鐘回撥**: 如果系統時鐘被向後調整，可能會產生重複的 ID。一個簡單的處理方式是，如果發現當前時間小於上次記錄的時間，就拋出異常。
*/

// 解答 (Solution)
class SnowflakeIDGenerator {
private:
    const int64_t epoch = 1609459200000L; // 紀元時間: 2021-01-01 00:00:00 UTC

    const int64_t machine_id_bits = 10;
    const int64_t sequence_bits = 12;

    const int64_t max_machine_id = -1L ^ (-1L << machine_id_bits);
    const int64_t max_sequence = -1L ^ (-1L << sequence_bits);

    const int64_t machine_id_shift = sequence_bits;
    const int64_t timestamp_shift = sequence_bits + machine_id_bits;

    int64_t machine_id;
    int64_t sequence = 0L;
    int64_t last_timestamp = -1L;

    std::mutex mtx;

    int64_t getCurrentTimeMillis() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }

    int64_t tilNextMillis(int64_t lastTimestamp) {
        int64_t timestamp = getCurrentTimeMillis();
        while (timestamp <= lastTimestamp) {
            timestamp = getCurrentTimeMillis();
        }
        return timestamp;
    }

public:
    SnowflakeIDGenerator(int64_t machineId) : machine_id(machineId) {
        if (machine_id < 0 || machine_id > max_machine_id) {
            throw std::invalid_argument("Machine ID is out of range.");
        }
    }

    int64_t generate() {
        std::lock_guard<std::mutex> lock(mtx);

        int64_t timestamp = getCurrentTimeMillis();

        if (timestamp < last_timestamp) {
            throw std::runtime_error("Clock moved backwards. Refusing to generate id.");
        }

        if (timestamp == last_timestamp) {
            sequence = (sequence + 1) & max_sequence;
            if (sequence == 0) {
                timestamp = tilNextMillis(last_timestamp);
            }
        } else {
            sequence = 0L;
        }

        last_timestamp = timestamp;

        return ((timestamp - epoch) << timestamp_shift) |
               (machine_id << machine_id_shift) |
               sequence;
    }
};


// main 函式用於測試
int main() {
    std::cout << "--- Testing Snowflake ID Generator ---" << std::endl;
    try {
        SnowflakeIDGenerator generator(101); // Machine ID 101

        const int num_ids_to_generate = 1000;
        std::set<int64_t> id_set;

        std::cout << "Generating " << num_ids_to_generate << " IDs..." << std::endl;
        for (int i = 0; i < num_ids_to_generate; ++i) {
            id_set.insert(generator.generate());
        }

        std::cout << "Generated " << id_set.size() << " unique IDs." << std::endl;
        if (id_set.size() == num_ids_to_generate) {
            std::cout << "Uniqueness test passed." << std::endl;
        } else {
            std::cout << "Uniqueness test failed." << std::endl;
        }

        std::cout << "\nFirst 5 generated IDs:" << std::endl;
        auto it = id_set.begin();
        for(int i=0; i<5 && it != id_set.end(); ++i, ++it) {
            std::cout << *it << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    std::cout << "--- Test Ended ---" << std::endl;
    return 0;
}