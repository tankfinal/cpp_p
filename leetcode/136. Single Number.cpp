/*
* 題目：LeetCode 136. Single Number
 * 連結：https://leetcode.com/problems/single-number/
 *
 * 【Embedded 對應】
 * - XOR 累積：對應暫存器遮罩/去重；每對相同值抵消為 0，剩下的即為唯一值。
 *
 * 敘述：陣列中每個元素出現兩次，僅一個元素出現一次，找出它。
 *
 * 解法：全陣列 XOR。
 * 複雜度：時間 O(n)、空間 O(1)
 */

#include <vector>
using namespace std;

class Solution {
public:
    int singleNumber(vector<int>& nums) {
        int x = 0;
        for (int v : nums) x ^= v;
        return x;
    }
};
