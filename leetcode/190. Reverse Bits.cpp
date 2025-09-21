/*
* 題目：LeetCode 190. Reverse Bits
 * 連結：https://leetcode.com/problems/reverse-bits/
 *
 * 【Embedded 對應】
 * - 位元重排：常見於端序/位序轉換、外設資料位元顛倒；用遮罩/移位分組交換更接近硬體思維。
 *
 * 敘述：將 32 位無號整數的位元順序反轉。
 *
 * 解法 A（位元遮罩分組交換；高速、常數時間）：
 *  - 先交換奇偶位，再交換每 2 位、每 4 位、每 8 位、每 16 位。
 * 複雜度：時間 O(1)、空間 O(1)
 */

#include <cstdint>
using namespace std;

class Solution {
public:
    uint32_t reverseBits(uint32_t n) {
        n = ((n >> 1)  & 0x55555555u) | ((n & 0x55555555u) << 1);
        n = ((n >> 2)  & 0x33333333u) | ((n & 0x33333333u) << 2);
        n = ((n >> 4)  & 0x0F0F0F0Fu) | ((n & 0x0F0F0F0Fu) << 4);
        n = ((n >> 8)  & 0x00FF00FFu) | ((n & 0x00FF00FFu) << 8);
        n = (n >> 16) | (n << 16);
        return n;
    }
};

/* 解法 B（迴圈 32 次；簡單但略慢）
class Solution {
public:
    uint32_t reverseBits(uint32_t n) {
        uint32_t res = 0;
        for (int i = 0; i < 32; ++i) {
            res = (res << 1) | (n & 1u);
            n >>= 1;
        }
        return res;
    }
};
*/
