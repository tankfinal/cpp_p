/*
* 題目：LeetCode 20. Valid Parentheses
 * 連結：https://leetcode.com/problems/valid-parentheses/
 *
 * 【Embedded 對應】
 * - 基礎 stack 匹配：像解析小語法/封包邊界；LIFO 保證配對順序。
 */
class Solution {
public:
    bool isValid(string s) {
        stack<char> st;
        if (s.size() < 2) {
            return false;
        }

        for (char s1: s) {
            if (s1 == '(' || s1 == '{' || s1 == '[') {
                st.push(s1);
            } else {
                if (st.empty()) {
                    return false;
                }
                if ((s1 == '}' && st.top() == '{') ||
                    (s1 == ']' && st.top() == '[') ||
                    (s1 == ')' && st.top() == '(')) {
                    st.pop();
                } else {
                    return false;
                }
            }
        }
        return st.empty();
    }
};
