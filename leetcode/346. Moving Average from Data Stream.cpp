/*
* 題目：LeetCode 346. Moving Average from Data Stream
 * 連結：https://leetcode.com/problems/moving-average-from-data-stream/
 *
 * 【Embedded 對應】
 * - 固定長度 FIFO 平滑器：常見於訊號去雜訊/移動平均；O(1) 更新、O(1) 空間（相對於窗口大小）。
 */

#include <queue>
using namespace std;

class MovingAverage {
    int cap;
    queue<int> q;
    long long sum = 0;
public:
    MovingAverage(int size): cap(size) {}
    double next(int val) {
        sum += val;
        q.push(val);
        if ((int)q.size() > cap) { sum -= q.front(); q.pop(); }
        return (double)sum / q.size();
    }
};
