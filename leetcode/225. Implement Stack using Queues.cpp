/*
* 題目：LeetCode 225. Implement Stack using Queues
 * 連結：https://leetcode.com/problems/implement-stack-using-queues/
 *
 * 【Embedded 對應】
 * - 以單一 FIFO 模擬 LIFO：push 後把前面的元素旋轉到尾端以維持「新元素在隊首」。
 * - 單佇列、無額外空間；可用固定容量環形佇列替代 std::queue。
 *
 * 複雜度：push O(n)，pop/top O(1)
 */

#include <queue>
using namespace std;

class MyStack {
    queue<int> q;

public:
    MyStack() {
    }

    void push(int x) {
        q.push(x);
        int n = (int) q.size();
        for (int i = 0; i < n - 1; ++i) {
            q.push(q.front());
            q.pop();
        }
    }

    int pop() {
        int v = q.front();
        q.pop();
        return v;
    }

    int top() const { return const_cast<queue<int> &>(q).front(); }
    bool empty() const { return q.empty(); }
};
