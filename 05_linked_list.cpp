#include <iostream>
#include <initializer_list>

/*
──────────────────────────────────────────────────────────────────────────────
[面試前備忘錄｜Google Embedded 視角]
• C++ 沒有 GC，Linked List 節點需用 new/delete 管理記憶體。
• 單向 Linked List：每個節點只知道下一個 (Node* next)。
  - 插入/刪除頭部 O(1)。
  - 查找元素 O(n)，cache 不友善。
• Reverse：用三個指標 (prev/cur/nxt) 一次遍歷反轉 → O(n)，O(1) 空間。
• Embedded 環境：常避免頻繁 new/delete → 改用固定大小 pool 或靜態陣列模擬。
──────────────────────────────────────────────────────────────────────────────
[常見追問]
Q1: 為什麼不用 array 實作？
A1: array 隨機存取快，但插入刪除 O(n)。Linked List 插入/刪除 O(1)，但 cache 不友善。

Q2: 為什麼 reverse 要用三個指標？
A2: 因為單向鏈表失去 next 就斷鏈，必須暫存 nxt 才能前進。

Q3: 釋放記憶體怎麼做？
A3: 手動 delete；或在 embedded 用 pre-allocated pool，避免 heap 碎片。

Q4: Java 版跟 C++ 版差別？
A4: Java 有 GC，所以只要改 next；C++ 必須顧及 new/delete。

──────────────────────────────────────────────────────────────────────────────
[陷阱備忘]
• 忘記暫存 nxt → 斷鏈，list 損壞。
• 忘記 delete → memory leak。
• delete 同一節點兩次 → 未定義行為。
• 在 embedded/RTOS 要小心 heap fragmentation，建議 pool allocator。
──────────────────────────────────────────────────────────────────────────────
*/

// 節點結構
struct Node {
    int val;      // 節點值
    Node* next;   // 下一個節點指標
    Node(int v) : val(v), next(nullptr) {} // 建構子
};

// 在前端插入節點
Node* push_front(Node* head, int v) {
    Node* n = new Node(v); // 在 heap 配置新節點
    n->next = head;        // 新節點指向舊 head
    return n;              // 回傳新的 head
}

// 反轉單向鏈結串列
Node* reverse_list(Node* head) {
    Node* prev = nullptr;  // 前一個節點
    Node* cur  = head;     // 當前節點
    while (cur) {
        Node* nxt = cur->next; // 暫存下一個
        cur->next = prev;      // 反轉指向
        prev = cur;            // prev 前進
        cur  = nxt;            // cur 前進
    }
    return prev; // 新 head
}

// 列印 Linked List
void print(const Node* h) {
    while (h) {
        std::cout << h->val << " ";
        h = h->next;
    }
    std::cout << "\n";
}

int main() {
    Node* head = nullptr;

    // 建立 list: 4 -> 3 -> 2 -> 1
    int xs[] = {1, 2, 3, 4};
    for (int i = 0; i < 4; i++) {
        head = push_front(head, xs[i]);
    }
    print(head);

    // 反轉後: 1 -> 2 -> 3 -> 4
    head = reverse_list(head);
    print(head);

    // 手動釋放記憶體（無 GC）
    while (head) {
        Node* t = head;
        head = head->next;
        delete t;
    }

    /*
    ───────────────────────────────────────────────────────────
    [總結口條（可直接講）]
    • 這是單向 Linked List，push_front O(1)，reverse O(n)。
    • 用 prev/cur/nxt 三指標確保不斷鏈，反轉成功。
    • 在 C++ 必須手動 delete；Java 有 GC 不用管。
    • 在 embedded 建議用固定大小 pool 代替 heap，避免碎片。
    ───────────────────────────────────────────────────────────
    */
    return 0;
}
