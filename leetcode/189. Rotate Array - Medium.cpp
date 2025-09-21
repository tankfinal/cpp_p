/*
 * 題目：LeetCode 189. Rotate Array
 * 連結：https://leetcode.com/problems/rotate-array/
 *
 * 【Embedded 對應】
 * - 常見於「環形緩衝區重定位 / 視窗平移」：將最近 k 筆資料移到前段或做資料對齊。
 * - 反轉三步法為「原地 O(1) 空間、線性時間、連續寫入」→ SRAM 友善、快取命中高，無需 heap。
 * - 若需要嚴格最少搬動次數，可採「循環置換（cycle replacement）」；兩者皆為 O(n)/O(1)。
 * - k 可能大於 n 或為負：需做模運算與正規化，確保不做冤枉工與行為可預期。
 *
 * 敘述：
 * 給定整數陣列 nums 與整數 k，將陣列「向右旋轉」k 步（每一步將陣列最後一個元素移到開頭）。
 *
 * 解法 A（推薦）：反轉三步法 — O(n) 時間、O(1) 空間
 * 思路：
 *   1) 全體反轉
 *   2) 反轉前 k 段
 *   3) 反轉後 n-k 段
 * 正確性：將 nums 分成 A | B（B 長度為 k）。全體反轉得 A^R | B^R，再分段各自反回 → B | A，即右旋結果。
 */

#include <vector>
#include <algorithm>
using namespace std;

class Solution {
public:
    void rotate(vector<int>& nums, int k) {
        const int n = static_cast<int>(nums.size());
        if (n <= 1) return;

        // 將 k 步數壓到 [0, n-1]，並處理可能的負值（若題目保證非負，可省略第二行）
        k %= n;
        if (k < 0) k += n;       // 一般習慣寫成：k = (k % n + n) % n;
        if (k == 0) return;

        // 反轉三步法（reverse 的區間為半開 [first, last)）
        reverse(nums.begin(), nums.end());            // 步驟 1：全體反轉
        reverse(nums.begin(), nums.begin() + k);      // 步驟 2：反轉前 k
        reverse(nums.begin() + k, nums.end());        // 步驟 3：反轉後 n-k
    }
};

/*
 * Alternative：循環置換（Cycle Replacement）— O(n) / O(1)
 * 思路：從每個「循環起點」出發，沿著 (idx + k) % n 將元素逐一放回正確位置，每個元素搬一次。
 * 搬動次數更少，但實作較繁。
 *
#include <utility> // for std::swap
class Solution {
public:
    void rotate(vector<int>& nums, int k) {
        const int n = static_cast<int>(nums.size());
        if (n <= 1) return;
        k %= n; if (k < 0) k += n;
        if (k == 0) return;

        int moved = 0;
        for (int start = 0; moved < n; ++start) {
            int cur = start;
            int prev = nums[start];
            do {
                int next = (cur + k) % n;
                std::swap(prev, nums[next]); // 把 prev 放到正確位置，取回被覆蓋值
                cur = next;
                ++moved;
            } while (cur != start);
        }
    }
};
*/

/*
 * Alternative（直觀，但用額外空間）：輔助陣列 — O(n) 時間、O(n) 空間
 * 若在面試允許額外空間時可快速完成；嵌入式環境通常避免此法以節省 SRAM/heap。
 *
class Solution {
public:
    void rotate(vector<int>& nums, int k) {
        const int n = static_cast<int>(nums.size());
        if (n <= 1) return;
        k = (k % n + n) % n;
        if (k == 0) return;

        vector<int> tmp(n);
        for (int i = 0; i < n; ++i)
            tmp[(i + k) % n] = nums[i];
        nums.swap(tmp);
    }
};
*/
