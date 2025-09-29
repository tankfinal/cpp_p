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
public:
    queue<int> cur;
    size_t cursize;
    uint32_t sum;
    MovingAverage(int size) : cursize(size), sum(0) {}

    double next(int val) {
        cur.push(val);
        sum += val;
        while(cur.size() > cursize) {
            sum -= cur.front();
            cur.pop();
        }

        return (static_cast<double>(sum)) / cur.size();
    }
};

/**
 * Your MovingAverage object will be instantiated and called as such:
 * MovingAverage* obj = new MovingAverage(size);
 * double param_1 = obj->next(val);
 */
