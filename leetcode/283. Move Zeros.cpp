/*
* 題目：LeetCode 283. Move Zeroes
    * 連結：https://leetcode.com/problems/move-zeroes/
 *
 * 【Embedded 對應】
 * - 等價於「流式壓縮/濾除（stream compaction）」：把有效樣本（非 0）穩定搬到前段，尾段清零。
 * - 原地 O(1) 空間、順序穩定，適合 SRAM 有限、避免 heap；連續記憶體寫入具備良好 cache locality。
 * - 在 DMA/ISR 讀入後做資料清理/壓縮很常見；此法分支簡單、可預測性好。
 *
 * 解法：寫入指標（兩段式）
 * 複雜度：時間 O(n)、空間 O(1)
 */

#include <vector>
using namespace std;

class Solution {
public:
    void moveZeroes(vector<int>& nums) {
        int k = 0;                                    // 下一個非 0 的寫入位置
        for (int i = 0; i < static_cast<int>(nums.size()); ++i)
            if (nums[i] != 0) nums[k++] = nums[i];    // 穩定前移
        for (int i = k; i < static_cast<int>(nums.size()); ++i)
            nums[i] = 0;                              // 尾段清零
    }
};

/* Alternative：單趟交換（同 O(n)/O(1)，多 0 時寫入次數更少）
#include <algorithm>
class Solution {
public:
    void moveZeroes(vector<int>& nums) {
        int last = 0;
        for (int i = 0; i < static_cast<int>(nums.size()); ++i)
            if (nums[i] != 0) {
                if (i != last) std::swap(nums[i], nums[last]);
                ++last;
            }
    }
};
*/
