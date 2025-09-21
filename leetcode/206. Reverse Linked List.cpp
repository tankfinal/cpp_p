/*
* 題目：LeetCode 206. Reverse Linked List
 * 連結：https://leetcode.com/problems/reverse-linked-list/
 *
 * 【Embedded 對應】
 * - 指標基本功：單鏈表原地反轉；僅用 3 個指標（prev/curr/next）。
 *
 * // LeetCode 既有定義：
 * // struct ListNode { int val; ListNode *next; ListNode(): val(0), next(nullptr) {} ListNode(int x): val(x), next(nullptr) {} ListNode(int x, ListNode *n): val(x), next(n) {} };
 */

struct ListNode { int val; ListNode* next; ListNode(int x): val(x), next(nullptr) {} };

class Solution_206 {
public:
    ListNode* reverseList(ListNode* head) {
        ListNode *prev = nullptr, *cur = head;
        while (cur) {
            ListNode* nxt = cur->next;
            cur->next = prev;
            prev = cur;
            cur = nxt;
        }
        return prev;
    }
};
