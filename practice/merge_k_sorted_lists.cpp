// =================================================================
// 題目四: 合併 K 個已排序的鏈結串列 (Merge K Sorted Lists)
// =================================================================
#include <iostream>
#include <vector>
#include <queue>

/*
[題目描述]
給定一個包含 k 個已排序鏈結串列的陣列 lists，將它們合併為一個已排序的鏈結串列並回傳。

[思路]
這個問題的最優解是使用最小堆 (Min-Heap)，在 C++ 中由 `std::priority_queue` 實現。
1.  **最小堆**: 我們需要一個可以儲存鏈結串列節點指標 (`ListNode*`) 的最小堆。`std::priority_queue` 預設是最大堆，所以我們需要提供一個自訂的比較器 (Comparator) 來讓它根據節點的 `val` 進行最小堆排序。
2.  **初始化**: 將 k 個鏈結串列的頭節點（如果非空）全部放入最小堆中。
3.  **合併過程**:
    - 建立一個啞節點 (dummy node) 作為結果鏈結串列的頭部。
    - 當最小堆不為空時，重複以下步驟：
      a. 從堆頂取出最小的節點（當前所有串列中最小的元素）。
      b. 將此節點接到結果鏈結串列的尾部。
      c. 如果該節點還有下一個節點 (`next` 非空)，則將其下一個節點推入最小堆中。
    - 迴圈結束後，啞節點的下一個節點就是合併後鏈結串列的頭。
時間複雜度: O(N log k)，其中 N 是總節點數，k 是鏈結串列的數量。每次從堆中取出和放入節點的時間是 O(log k)。
*/

// 解答 (Solution)
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x = 0, ListNode* n = nullptr) : val(x), next(n) {}
};

class Solution {
public:

    // 1. 定義一個比較用的結構 (struct)
    struct CompareNode {
        // 重載 () 運算子
        bool operator()(ListNode* a, ListNode* b) {
            // 因為 priority_queue 預設是最大堆 (less)
            // 我們要反過來，讓 val 大的節點「優先權較低」，形成最小堆
            return a->val > b->val;
        }
    };

    ListNode* mergeKLists(std::vector<ListNode*>& lists) {

        std::priority_queue<ListNode*, std::vector<ListNode*>, CompareNode> min_heap;

        for (ListNode* list : lists) {
            if (list) {
                min_heap.push(list);
            }
        }

        ListNode dummy;
        ListNode* current = &dummy;

        while (!min_heap.empty()) {
            ListNode* node = min_heap.top();
            min_heap.pop();

            current->next = node;
            current = current->next;

            if (node->next) {
                min_heap.push(node->next);
            }
        }

        return dummy.next;
    }
};

// 輔助函式：建立鏈結串列
ListNode* createList(const std::vector<int>& vals) {
    ListNode dummy;
    ListNode* current = &dummy;
    for (int val : vals) {
        current->next = new ListNode(val);
        current = current->next;
    }
    return dummy.next;
}

// 輔助函式：印出鏈結串列
void printList(ListNode* head) {
    while (head) {
        std::cout << head->val << " -> ";
        head = head->next;
    }
    std::cout << "nullptr" << std::endl;
}

// main 函式用於測試
int main() {
    std::cout << "--- Testing Merge K Sorted Lists ---" << std::endl;

    std::vector<ListNode*> lists;
    lists.push_back(createList({1, 4, 5}));
    lists.push_back(createList({1, 3, 4}));
    lists.push_back(createList({2, 6}));

    std::cout << "Input lists:" << std::endl;
    for(size_t i = 0; i < lists.size(); ++i) {
        std::cout << "List " << i+1 << ": ";
        printList(lists[i]);
    }

    Solution sol;
    ListNode* merged_head = sol.mergeKLists(lists);

    std::cout << "\nMerged list:" << std::endl;
    printList(merged_head);
    std::cout << "Expected: 1 -> 1 -> 2 -> 3 -> 4 -> 4 -> 5 -> 6 -> nullptr" << std::endl;

    std::cout << "--- Test Ended ---" << std::endl;
    return 0;
}