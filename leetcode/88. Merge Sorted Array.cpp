/*
* 題目：LeetCode 88. Merge Sorted Array
 * 連結：https://leetcode.com/problems/merge-sorted-array/
 *
 * 【Embedded 對應】
 * - 原地尾端合併（in-place, from back）：三指標逆向寫回，避免額外緩衝區。
 * - 連續寫入、無動態配置，利於快取與 SRAM 限制。
 *
 * 敘述：給定兩個非遞減排序陣列 nums1(m有效長度) 與 nums2(n)，將 nums2 合併進 nums1。
 *
 * 解法：i 指向 nums1 有效尾、j 指向 nums2 尾、k 指向 nums1 寫入尾，從大到小寫入。
 * 複雜度：時間 O(m+n)、空間 O(1)
 */

#include <vector>
using namespace std;

class Solution {
public:
    void merge(vector<int> &nums1, int m, vector<int> &nums2, int n) {
        int i = m - 1, j = n - 1, k = m + n - 1;
        while (j >= 0) {
            // 只要 nums2 還有元素就必須處理
            if (i >= 0 && nums1[i] > nums2[j]) nums1[k--] = nums1[i--];
            else nums1[k--] = nums2[j--];
        }
    }
};
