/*
* 題目：LeetCode 121. Best Time to Buy and Sell Stock
 * 連結：https://leetcode.com/problems/best-time-to-buy-and-sell-stock/
 *
 * 【Embedded 對應】
 * - 單通道「前綴最小值」掃描：像在 ISR/資料流中追蹤「目前最低讀值」與「即時最大差」。
 * - O(1) 狀態、O(n) 單趟，無 heap；適合低延遲、SRAM 受限環境。
 *
 * 敘述：給定每日股價，求一次買賣的最大利潤（買在前、賣在後）。
 *
 * 解法：維護「目前最低價 minp」與「最佳利潤 best」，一路更新。
 * 複雜度：時間 O(n)、空間 O(1)
 */

#include <vector>
#include <climits>
using namespace std;

class Solution {
public:
    int maxProfit(vector<int> &prices) {
        int minp = INT_MAX, best = 0;
        for (int p: prices) {
            if (p < minp) minp = p;
            if (p - minp > best) best = p - minp;
        }
        return best;
    }
}

/* 也可用「prefixMin + 單趟」等價寫法（或先預算 prefixMin 陣列後再掃描）。*/
