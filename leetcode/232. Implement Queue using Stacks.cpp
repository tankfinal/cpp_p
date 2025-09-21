/*
* 題目：LeetCode 232. Implement Queue using Stacks
 * 連結：https://leetcode.com/problems/implement-queue-using-stacks/
 *
 * 【Embedded 對應】
 * - 以兩個 LIFO 模擬 FIFO：像「寫入緩衝 + 讀出緩衝」；只在 out 為空時做批量轉移，均攤 O(1)。
 * - 無動態大小時可改用固定容量陣列實作兩個 stack（避免 heap）。
 *
 * 解法：in 負責 push；pop/peek 時若 out 空則把 in 全部倒到 out 再取。
 * 複雜度：均攤 O(1)
 */

#include <stack>
using namespace std;

class MyQueue {
    stack<int> in, out;
    void shift() {
        if (out.empty()) {
            while (!in.empty()) { out.push(in.top()); in.pop(); }
        }
    }
public:
    MyQueue() {}
    void push(int x) { in.push(x); }
    int pop() { shift(); int v = out.top(); out.pop(); return v; }
    int peek() { shift(); return out.top(); }
    bool empty() const { return in.empty() && out.empty(); }
};
