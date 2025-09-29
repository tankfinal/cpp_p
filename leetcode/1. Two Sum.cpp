/*
* 題目：LeetCode 1. Two Sum
 * 連結：https://leetcode.com/problems/two-sum/
 *
 * 【Embedded 對應】
 * - 時間/空間取捨：單趟 O(n) 找補數，適合「低延遲」需求；但 unordered_map 會動態配置（heap）。
 *   在 MCU/RTOS 上若不允許 heap，可改「固定容量開放定址哈希」（std::array 實作）或「排序+雙指針（O(n log n)）」保留索引。
 * - 可預測性：平均 O(1) 查找（雜湊），最壞退化需注意；可用 reserve() 降 rehash、平滑延遲尖峰。
 * - 記憶體：雜湊表 cache locality 較差，但一次性掃描可接受；n 小時 O(n^2) 也常足夠（免額外空間）。
 *
 * 敘述：
 * 給定整數陣列 nums 與整數 target，回傳兩個索引 i、j（i ≠ j），使得 nums[i] + nums[j] == target。
 *
 * 解法：單趟雜湊（先查後放避免自用）
 * 複雜度：時間 O(n)、空間 O(n)
 */

#include <vector>
#include <unordered_map>
using namespace std;

class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        unordered_map<int, int> pos;      // 值 -> 索引
        pos.reserve(nums.size());         // 小優化：避免多次 rehash
        for (int i = 0; i < nums.size(); ++i) {
            int need = target - nums[i];
            auto it = pos.find(need);
            if (it != pos.end()) {        // 找到了互補值
                return {it->second, i};   // 舊索引在前，當前在後
            }
            pos[nums[i]] = i;             // 邊掃邊記錄
        }
        return {}; // 題目保證有解時可不需要；保險起見留空結果
    }
};