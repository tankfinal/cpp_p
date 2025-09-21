/*
* 題目：LeetCode 191. Number of 1 Bits
 * 連結：https://leetcode.com/problems/number-of-1-bits/
 *
 * 【Embedded 對應】
 * - 位元計數：典型暫存器位操作；n &= (n - 1) 每次清掉最低位的 1（Brian Kernighan 演算法）。
 *
 * 敘述：回傳無號 32 位整數二進位表示中 '1' 的個數。
 *
 * 解法：反覆 n &= (n - 1) 直到 n 為 0。
 * 複雜度：時間 O(設定位數) ≤ O(32)，空間 O(1)
 */

#include <cstdint>
using namespace std;

class Solution {
public:
    int hammingWeight(uint32_t n) {
        int cnt = 0;
        while (n) {
            n &= (n - 1); // 刪掉最低位的 1
            ++cnt;
        }
        return cnt;
    }
};

/* 亦可用編譯器內建：
   return __builtin_popcount(n);  // GCC/Clang
*/
