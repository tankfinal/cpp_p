/*
* 題目：LeetCode 155. Min Stack
 * 連結：https://leetcode.com/problems/min-stack/
 *
 * 【Embedded 對應】
 * - 主棧 + 最小值棧：額外 O(1) 狀態追蹤當前最小值；避免遍歷找最小。
 */

#include <stack>
using namespace std;

class MinStack {
    stack<int> s, mins;
public:
    MinStack() {}
    void push(int x) {
        s.push(x);
        if (mins.empty() || x <= mins.top()) mins.push(x);
    }
    void pop() {
        if (s.top() == mins.top()) mins.pop();
        s.pop();
    }
    int top() const { return const_cast<stack<int>&>(s).top(); }
    int getMin() const { return const_cast<stack<int>&>(mins).top(); }
};
