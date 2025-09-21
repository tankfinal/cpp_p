#include <iostream>
#include <array>
#include <vector>
#include <string>

/*
──────────────────────────────────────────────────────────────────────────────
[面試前備忘錄｜Google Embedded 視角]
• 容器選擇：
  - std::array<T,N>：固定大小，stack 配置，無 heap，生命週期可預測 → 嵌入式首選。
  - std::vector<T>：動態大小，heap 配置，彈性強，但 embedded 要小心：
    可能造成 heap 碎片與不可預測延遲。
  - std::string：封裝動態字串，通常有小字串最佳化 (SSO)。
    在 embedded，若只需短字串，考慮 std::array<char,N>。

• 迭代器 (iterator)：提供統一介面，泛型演算法（如 std::sort）都靠它。
  - begin() → 第一個元素
  - end()   → 尾後元素 (不可解參考，只能當邊界)

• vector.reserve vs resize：
  - reserve(n)：只改 capacity，不改 size → 減少 realloc。
  - resize(n)：改 size → 可能構造新元素。

• Java 對照：
  - array  ≈ int[]
  - vector ≈ ArrayList<Integer>
  - string ≈ String (但 Java String 不可變，C++ std::string 可變)

──────────────────────────────────────────────────────────────────────────────
[常見追問（你可口述）]
Q1: 為何 embedded 不建議用 vector？
A1: 因為 vector 用 heap，push_back 可能 realloc，導致不可預測延遲；
    在即時系統中會破壞 determinism。

Q2: 那要怎麼安全用 vector？
A2: 先 reserve 足夠容量，避免 runtime realloc；
    或在受控平台 (Linux/RTOS + MMU) 可以接受。

Q3: string 與 Java String 有什麼差別？
A3: Java String 不可變；C++ std::string 可變動，通常有 SSO，
    短字串存在內部 buffer，不會配置 heap。

Q4: 迭代器 vs index 有什麼差？
A4: 迭代器可用於泛型演算法 (sort, find, transform)；
    index 只適合隨機存取容器 (array, vector)。

──────────────────────────────────────────────────────────────────────────────
[陷阱備忘]
• 不要解參考 v.end() → 未定義行為。
• reserve 不會初始化元素；resize 才會。
• vector.insert 可能使迭代器失效 → 小心用。
• string += 在 Java 會 new 新字串，但在 C++ 是就地修改。
──────────────────────────────────────────────────────────────────────────────
*/

int main() {
    // ── std::array：固定大小容器 ───────────────────────────────
    std::array<int,4> a = {1,2,3,4};
    for (int v : a) std::cout << v << " ";
    std::cout << "\n";
    // Java 對照：int[] a = {1,2,3,4};
    // 差異：std::array 知道自己長度 (.size())，能當 STL 容器使用。

    // ── std::vector：動態大小容器 ─────────────────────────────
    std::vector<int> v1;
    v1.push_back(22); // 在尾端新增元素（自動擴容）
    // Java 對照：ArrayList<Integer> v1 = new ArrayList<>(); v1.add(22);

    std::vector<int> v;
    v.reserve(4); // 預先保留空間，避免 push_back 多次 realloc → 減少拷貝/搬移成本
    v.push_back(10);
    v.push_back(30);
    v.insert(v.begin()+1, 99); // 在位置 1 插入元素 → vector 內部元素移動

    // ── 迭代器 (iterator) ────────────────────────────────────
    auto wrap_iter = v.begin();
    std::cout << "manual (first element via iterator): "
              << *wrap_iter << "\n";  // 等同 v[0] 或 v.front()
    wrap_iter += 1; // 迭代器前進一格 → 指向第二個元素

    std::cout << "manual (last element via back()): "
              << v.back() << "\n";

    // ── Range-based for：C++11 語法糖，等同 Java for-each ───
    for (auto x : v) {
        std::cout << x << "\n";
    }

    // ── vector 的屬性 ────────────────────────────────────────
    std::cout << "v.size()=" << v.size() << "\n";
    // .size() = 元素數量
    // .capacity() = 底層配置容量（大於等於 size）
    // .reserve(n) = 調整容量，避免反覆 realloc

    // ── std::string：動態字串 ───────────────────────────────
    std::string s = "C++";
    s += " vs Java"; // 支援 operator+=，就地修改（可變）
    std::cout << s << "\n";
    // Java 對照：String s = "C++"; s += " vs Java";
    // 差異：Java String 不可變，+= 會 new 新字串；
    //       C++ std::string 可變（SSO：短字串常存在 stack buffer）。

    return 0;
}
