#include <iostream>

/*
──────────────────────────────────────────────────────────────────────────────
[面試前備忘錄｜Google Embedded 視角]
• C++ 有「指標 (T*)」與「參考 (T&)」：
  - 指標：可為空、可換指向、可做算術（常用於陣列與記憶體映射 I/O）。
  - 參考：建立時必須綁定，之後不可再 reseat，語意更安全，常用於函式參數。
• Embedded 常見應用：
  - 指標：直接操作硬體暫存器、DMA buffer、環形佇列。
  - 參考：函式參數傳遞，避免拷貝，且語意比裸指標更清楚。
• nullptr（C++11 引入）：比傳統 NULL/0 更安全，有型別檢查。
• const pointer vs pointer-to-const：常被考，分清「不能改值」vs「不能換指向」。
──────────────────────────────────────────────────────────────────────────────
[常見追問]
Q1: 什麼時候用指標，什麼時候用參考？
A1:
  - 指標：需要可空、需要換指向、需要算術（遍歷記憶體/陣列）。
  - 參考：必須存在、不可空、語意為「別名」，API 清楚安全。

Q2: Embedded 為什麼要懂 pointer arithmetic？
A2: 操作連續記憶體（陣列、UART FIFO、MMIO），透過指標算術可直接存取硬體位址。

Q3: const int* vs int* const 差在哪？
A3:
  - const int* p: 指向的值唯讀，指標可換。
  - int* const p: 指標本身固定，值可改。
──────────────────────────────────────────────────────────────────────────────
*/

int main() {
    int a = 10;

    // ── 指標與參考 ─────────────────────────────────────────────
    int* p = &a;          // p = 指標，保存變數 a 的位址
    int& r = a;           // r = 參考，a 的別名

    std::cout << "a=" << a << " *p=" << *p << " r=" << r << "\n";
    // *p 解參考 (dereference)：取出 p 指向位址的值

    *p = 20;              // 經由指標修改值 → a 被改成 20
    std::cout << "after *p=20, a=" << a << " r=" << r << "\n";
    // 因為 r 是 a 的別名，所以 r 也同步改變

    // ── 陣列與指標運算 (Pointer Arithmetic) ─────────────────────
    int arr[5] = {1,2,3,4,5};
    int* p0 = arr;        // 陣列在大部分表達式會 decay 成指向首元素的指標
    std::cout << "*(p0+2)=" << *(p0+2) << " (pointer arithmetic)\n";
    // p0+2 → 跳到第 3 個元素 (以 sizeof(int) 為位移單位)

    // ── nullptr ───────────────────────────────────────────────
    int* np = nullptr;    // C++11 引入，比 NULL/0 更安全
    if (!np) std::cout << "np is nullptr\n";
    // Java 類比：等同 "null" 參考

    // ── const pointer vs pointer-to-const ─────────────────────
    const int ci = 7;

    const int* pc = &ci;
    // "pointer-to-const"：透過 pc 不能修改值，但 pc 可改指向
    // *pc = 8;   // ❌ 編譯錯誤
    pc = &a;     // ✅ OK，pc 可換指向

    int* const cp = &a;
    // "const pointer"：指標不可換，但可修改指向的值
    *cp = 99;    // ✅ OK，修改 a 的值
    // cp = &ci; // ❌ 編譯錯誤，cp 不能改指向

    /*
    ───────────────────────────────────────────────────────────
    [總結備忘]
    • 指標 (T*): 可空、可換、可算術 → 適合底層/硬體操作。
    • 參考 (T&): 不能 reseat、不可空 → 適合 API 參數傳遞。
    • nullptr: 型別安全的空指標。
    • const pointer vs pointer-to-const: 分清「不能改值」vs「不能換位址」。
    • Embedded 常見 follow-up:
      - 操作記憶體映射暫存器：用 volatile T*。
      - 寫 driver/ISR 時：指標 vs 參考差異 → deterministic & safety。
    ───────────────────────────────────────────────────────────
    */

    return 0;
}
