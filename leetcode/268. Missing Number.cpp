/*
* 題目：LeetCode 268. Missing Number
 * 連結：https://leetcode.com/problems/missing-number/
 *
 * 【Embedded 對應】
 * - XOR 累積檢查：類似暫存器位元校驗/奇偶校驗；O(1) 狀態、O(n) 單趟。
 *
 * 敘述：給定包含 [0..n] 中 n 個不同數字的陣列，找出遺失的那個數。
 *
 * 解法（XOR）：
 *  - x ^ x = 0，x ^ 0 = x；把 0..n 與所有 nums[i] 逐一 XOR，剩下的就是缺失值。
 * 複雜度：時間 O(n)、空間 O(1)
 */

#include <vector>
using namespace std;

class Solution {
public:
    int missingNumber(vector<int> &nums) {
        int n = static_cast<int>(nums.size());
        int x = 0;
        for (int i = 0; i <= n; ++i) x ^= i; // 取得全部的xor總數
        for (int v: nums) x ^= v; // 扣到最後剩下的就是少的
        return x;
    }
};
