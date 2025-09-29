/*
* 題目：LeetCode 26. Remove Duplicates from Sorted Array
 * 連結：https://leetcode.com/problems/remove-duplicates-from-sorted-array/
 *
 * 【Embedded 對應】
 * - 已排序資料的「去重壓縮」：常見於時間序事件/感測數據經排序後做去重，原地 O(1) 空間、連續寫入友善快取。
 * - 單通道、可預測的線性掃描，適合 ISR 後處理或低延遲資料管線；避免任何動態配置。
 *
 * 解法：雙指標（slow=寫入位 / fast=掃描位）
 * 複雜度：時間 O(n)、空間 O(1)
 */

#include <vector>
using namespace std;
class Solution {
public:
    int removeDuplicates(vector<int>& nums) {
        int w = 1;
        for (int r = 1 ; r < nums.size() ; ++r) {
            if (nums[r] != nums[w-1]) {
                nums[w] = nums[r];
                w++;
            }
        }
        return w;
    }
};