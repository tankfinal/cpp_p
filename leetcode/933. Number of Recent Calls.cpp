/*
* 題目：LeetCode 933. Number of Recent Calls
 * 連結：https://leetcode.com/problems/number-of-recent-calls/
 *
 * 【Embedded 對應】
 * - 窗口為 [t-3000, t] 的滑動計數：等同簡易限流器（sliding window 限流）。
 * - 以 FIFO 佇列保存時間戳，過期就彈出；O(1) 改動。
 */

#include <queue>
using namespace std;

class RecentCounter {
    queue<int> q;
public:
    RecentCounter() {}
    int ping(int t) {
        q.push(t);
        while (!q.empty() && q.front() < t - 3000) q.pop();
        return (int)q.size();
    }
};
