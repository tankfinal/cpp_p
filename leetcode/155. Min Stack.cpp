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
public:
    stack<int> st;
    stack<int> mins;
    MinStack() {}

    void push(int val) {
        st.push(val);
        if (mins.empty() || val <= mins.top()) {
            mins.push(val);
        }
    }
    void pop() {
        if (st.empty()) {
            throw runtime_error("empty stack");
        }
        if (st.top() == mins.top())
            mins.pop();
        st.pop();
    }

    int top() {
        if (st.empty()) {
            throw runtime_error("empty stack");
        }
        return st.top();
    }

    int getMin() {
        if (mins.empty()) {
            throw runtime_error("empty stack");
        }
        return mins.top();
    }
};

/**
 * Your MinStack object will be instantiated and called as such:
 * MinStack* obj = new MinStack();
 * obj->push(val);
 * obj->pop();
 * int param_3 = obj->top();
 * int param_4 = obj->getMin();
 */