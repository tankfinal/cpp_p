/*
* 題目：LeetCode 21. Merge Two Sorted Lists
 * 連結：https://leetcode.com/problems/merge-two-sorted-lists/
 *
 * 【Embedded 對應】
 * - 兩條已排序 buffer 串接：使用假頭節點（dummy）簡化邏輯；原地重用節點。
 */

struct ListNode2 { int val; ListNode2* next; ListNode2(int x): val(x), next(nullptr) {} };

class Solution_21 {
public:
    ListNode2* mergeTwoLists(ListNode2* a, ListNode2* b) {
        ListNode2 dummy(0), *tail = &dummy;
        while (a && b) {
            if (a->val <= b->val) { tail->next = a; a = a->next; }
            else { tail->next = b; b = b->next; }
            tail = tail->next;
        }
        tail->next = a ? a : b;
        return dummy.next;
    }
};
