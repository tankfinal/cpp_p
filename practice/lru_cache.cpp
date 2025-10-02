// =================================================================
// 題目一: 實作 LRU 快取 (LRU Cache)
// =================================================================
#include <iostream>
#include <list>
#include <unordered_map>
#include <utility> // for std::pair

/*
[題目描述]
設計並實作一個 LRUCache (Least Recently Used Cache) 類別，支援以下操作：
1.  LRUCache(capacity): 初始化快取，給定一個正整數的容量。
2.  get(key): 如果 key 存在於快取中，則回傳其 value，否則回傳 -1。每次 get 操作都會將該 key 標記為「最近使用」。
3.  put(key, value): 如果 key 不存在，則寫入 key-value。如果 key 已存在，則更新 value。如果寫入後超出容量，則必須淘汰「最久未使用」的資料。
所有操作都應盡可能在 O(1) 時間複雜度內完成。



[思路]
要實現 O(1) 的 get 和 put，需要結合兩種資料結構：
1.  `std::unordered_map` (雜湊表): 用於儲存 key 到一個指向鏈結串列節點的迭代器的映射。這確保了我們可以 O(1) 找到任何 key 對應的資料。
2.  `std::list` (雙向鏈結串列): 用於記錄資料的使用順序。我們將「最近使用」的資料放在鏈結串列的頭部，「最久未使用」的資料放在尾部。
   - `get` 操作: 透過 map 找到資料，如果存在，就將其對應的節點移動到 list 的頭部，並回傳 value。
   - `put` 操作:
     - 如果 key 已存在，更新 value，並將其節點移到 list 頭部。
     - 如果 key 不存在，先檢查容量。若已滿，則移除 list 尾部節點（最久未使用），並從 map 中刪除對應的 key。然後，在 list 頭部插入新的 key-value，並在 map 中建立新的映射。
*/

class LRUCache {
public:
    int capacity;
    std::list<std::pair<int, int> > list;
    std::unordered_map<int, std::list<std::pair<int, int>>::iterator> map;

    explicit LRUCache(int capacity) : capacity(capacity) {}

    int get(int key) {
        auto it = map.find(key);
        if (it == map.end()) {
            return -1;
        }
        list.splice(list.begin(), list, it->second);
        return it->second->second;
    }

    void put(int key, int value) {
        auto it = map.find(key);
        if (it != map.end()) {
            it->second->second = value;
            list.splice(list.begin(), list, it->second);
        }
        if (list.size() == capacity) {
            auto wait_removed_pair = list.back().second;
            map.erase(wait_removed_pair);
            list.pop_back();
        }
        list.push_front({key, value});
        map[key] = list.begin();
    }

    void printCacheState() const {
        std::cout << "  Cache State (MRU -> LRU): ";
        if (list.empty()) {
            std::cout << "Empty" << std::endl;
            return;
        }
        for (const auto& pair : list) {
            std::cout << "[" << pair.first << ":" << pair.second << "] ";
        }
        std::cout << std::endl;
    }
};

int main() {
    std::cout << "--- Testing LRU Cache ---" << std::endl;
    LRUCache cache(2); // 容量為 2

    cache.put(1, 1);
    std::cout << "Put(1, 1)" << std::endl;
    cache.printCacheState();
    std::cout << "-------------------------" << std::endl;

    cache.put(2, 2);
    std::cout << "Put(2, 2)" << std::endl;
    cache.printCacheState();
    std::cout << "-------------------------" << std::endl;

    std::cout << "Get(1): " << cache.get(1) << " (Expected: 1)" << std::endl; // 回傳 1, 現在 (1,1) 是最新
    cache.printCacheState();
    std::cout << "-------------------------" << std::endl;

    cache.put(3, 3); // 執行 put(3,3)，容量已滿，應淘汰 (2,2)
    std::cout << "Put(3, 3), key 2 should be evicted" << std::endl;
    cache.printCacheState();
    std::cout << "Get(2): " << cache.get(2) << " (Expected: -1)" << std::endl; // 回傳 -1 (已被淘汰)
    std::cout << "-------------------------" << std::endl;

    cache.put(4, 4); // 執行 put(4,4)，容量已滿，應淘汰 (1,1)
    std::cout << "Put(4, 4), key 1 should be evicted" << std::endl;
    cache.printCacheState();
    std::cout << "Get(1): " << cache.get(1) << " (Expected: -1)" << std::endl; // 回傳 -1 (已被淘汰)
    std::cout << "Get(3): " << cache.get(3) << " (Expected: 3)" << std::endl;
    cache.printCacheState();
    std::cout << "Get(4): " << cache.get(4) << " (Expected: 4)" << std::endl;
    cache.printCacheState();

    std::cout << "\n--- Test Ended ---" << std::endl;
    return 0;
}

// 解答 (Solution)
// class LRUCache {
// private:
//     int capacity;
//     std::list<std::pair<int, int>> cacheList; // first: key, second: value
//     std::unordered_map<int, std::list<std::pair<int, int>>::iterator> cacheMap;
//
// public:
//     LRUCache(int capacity) : capacity(capacity) {}
//
//     int get(int key) {
//         auto it = cacheMap.find(key);
//         if (it == cacheMap.end()) {
//             return -1;
//         }
//         // splice 可以在 O(1) 將節點移到頭部
//         cacheList.splice(cacheList.begin(), cacheList, it->second);
//         return it->second->second;
//     }
//
//     int get(int key) {
//         auto it = cacheMap.find(key);
//         if (it == cacheMap.end()) {
//             return -1;
//         }
//         return it->second->second;
//     }
//
//     int put(int key, int value) {
//         auto it = cacheMap.find(key);
//         if (it != cacheMap.end()) {
//             it->second->second = value;
//             cacheList.splice(cacheList.begin(), cacheList, it-> second);
//         }
//         if (cacheList.size() >= capacity) {
//             auto cache_list = cacheList.back().first;
//             cacheMap.erase(cache_list);
//             cacheList.pop_back();
//         }
//         cacheList.push_front({key, value});
//         cacheMap[key] = cacheList.begin();
//     }
//
//     void put(int key, int value) {
//         auto it = cacheMap.find(key);
//         if (it != cacheMap.end()) {
//             it->second->second = value;
//             cacheList.splice(cacheList.begin(), cacheList, it->second);
//         } else {
//             if (cacheList.size() >= capacity) {
//                 int key_to_remove = cacheList.back().first;
//                 cacheMap.erase(key_to_remove);
//                 cacheList.pop_back();
//             }
//             cacheList.push_front({key, value});
//             cacheMap[key] = cacheList.begin();
//         }
//     }
//
//     // 新增的函式：打印 cacheList 的內容
//     // 鏈結串列的第一個元素是「最近使用 (MRU)」，最後一個是「最久未使用 (LRU)」
     // void printCacheState() const {
     //     std::cout << "  Cache State (MRU -> LRU): ";
     //     if (cacheList.empty()) {
     //         std::cout << "Empty" << std::endl;
     //         return;
     //     }
     //     for (const auto& pair : cacheList) {
     //         std::cout << "[" << pair.first << ":" << pair.second << "] ";
     //     }
     //     std::cout << std::endl;
     // }
// };
//
//
// // main 函式用於測試

