/*
* 題目：LeetCode 20. Valid Parentheses
 * 連結：https://leetcode.com/problems/valid-parentheses/
 *
 * 【Embedded 對應】
 * - 基礎 stack 匹配：像解析小語法/封包邊界；LIFO 保證配對順序。
 */

#include <stack>
#include <string>
using namespace std;

class Solution_20 {
public:
    bool isValid(string s) {
        stack<char> st;
        for (char c : s) {
            if (c=='(' || c=='[' || c=='{') st.push(c);
            else {
                if (st.empty()) return false;
                char o = st.top(); st.pop();
                if ((o=='(' && c!=')') || (o=='[' && c!=']') || (o=='{' && c!='}')) return false;
            }
        }
        return st.empty();
    }
};
