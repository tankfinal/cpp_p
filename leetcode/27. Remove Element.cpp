/*
* 題目：LeetCode 27. Remove Element
 * 連結：https://leetcode.com/problems/remove-element/
 *
 * 【Embedded 對應】
 * - 「就地過濾」：從樣本中剔除特定值（如錯誤碼、無效樣本）。原地 O(1) 空間，易於在 SRAM 中運作。
 * - 若寫入成本高（如 Flash/EEPROM），可用「交換法」減少寫入；在 RAM 中常用「寫入指標法」保序。
 *
 * 解法 A（保序、直觀）：寫入指標
 * 複雜度：時間 O(n)、空間 O(1)
 */

#include <vector>
using namespace std;

class Solution {
public:
    int removeElement(vector<int>& nums, int val) {
        int write = 0;
        const int n = static_cast<int>(nums.size());
        for (int i = 0; i < n; ++i)
            if (nums[i] != val)
                nums[write++] = nums[i];
        return write;                                 // 新長度；有效區間 nums[0..write-1]
    }
};

/* Alternative（不保序、寫入更少；val 稀少時更省）
#include <algorithm>
class Solution {
public:
    int removeElement(vector<int>& nums, int val) {
        int i = 0, end = static_cast<int>(nums.size());
        while (i < end) {
            if (nums[i] == val) { std::swap(nums[i], nums[end - 1]); --end; }
            else ++i;
        }
        return end;
    }
};
*/
