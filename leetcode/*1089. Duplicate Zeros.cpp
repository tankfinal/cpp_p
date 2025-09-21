/*
* 題目：LeetCode 1089. Duplicate Zeros
 * 連結：https://leetcode.com/problems/duplicate-zeros/
 *
 * 【Embedded 對應】
 * - 就地「擴張並截斷」：把所有 0 複製一次，超出陣列長度的部分直接截掉。
 * - 原地 O(1) 空間、單趟反向寫回，避免大量搬移與動態配置（MCU/RTOS 友善）。
 * - 兩指標從尾端寫回，分支簡單、快取/寫入行為可預測。
 *
 * 敘述：
 * 給定固定長度陣列 arr，對每個值為 0 的元素進行「複製」（再插入一個 0），
 * 但陣列長度固定，超出邊界的部分捨棄。請在原地修改陣列。
 *
 * 解法：計數 0 的數量，建立「虛擬尾指標」j = n + zeros - 1
 * 思路：
 *  1) 先掃一遍計算 zeros（要複製的 0 的個數）。
 *  2) 設 i 指向原陣列尾、j 指向「虛擬尾」（把複製後的陣列視為長度 n+zeros）。
 *  3) 由尾到頭把 arr[i] 寫到 arr[j]；若是 0，額外再寫一個 0（j-- 再寫）。
 *  4) 只在 j < n 時才真正寫入，避免越界；持續 i--, j--，直到結束。
 *
 * 正確性：
 *  - 反向寫可避免覆蓋尚未處理的資料；虛擬尾指標能自然處理「截斷」。
 *
 * 複雜度：時間 O(n)；空間 O(1)
 *
 * 範例：
 *  arr = [1,0,2,3,0,4,5,0]
 *  zeros=3, n=8 → j=10
 *  反向寫回後 arr = [1,0,0,2,3,0,0,4]
 */

#include <vector>
using namespace std;

class Solution {
public:
    void duplicateZeros(vector<int>& arr) {
        const int n = static_cast<int>(arr.size());
        int zeros = 0;
        for (int x : arr) if (x == 0) ++zeros;

        // i：原陣列最後一個有效索引
        // j：「虛擬擴張後」的最後一個索引（可能 >= n）
        int i = n - 1;
        long long j = static_cast<long long>(n) + zeros - 1; // 用長整數避免極端情況下溢位

        // 從尾端往前寫：只有在 j < n 時才真正寫入
        while (i >= 0 && j >= 0) {
            if (j < n) {
                arr[static_cast<int>(j)] = arr[i];
            }
            if (arr[i] == 0) {
                --j;
                if (j < n && j >= 0) {
                    arr[static_cast<int>(j)] = 0;   // 複製一個 0
                }
            }
            --i;
            --j;
        }
    }
};
