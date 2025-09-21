#include <iostream>
#include <string>

/*
──────────────────────────────────────────────────────────────────────────────
[面試前備忘錄｜Google Embedded 視角]
• 資源生命週期：C++ 用 RAII（離開 scope 立即解構），嵌入式無 GC，決定性釋放很重要。
• 參數傳遞策略：
  - 小型標量（int, double）：值傳遞（複製成本低，語意清楚）。
  - 大物件：以 const T& 只讀傳入（避免複製），若需要所有權可用 move。
• const 的意義：不只是唯讀，更是介面保證（幫助最佳化與 API 可預期性）。
• I/O 最佳化：sync_with_stdio(false) / cin.tie(nullptr) 提升 iostream 效率，但避免與 C stdio 混用。
• 風格：不用 `using namespace std;`，保留 `std::` 提升可讀性與避免衝突（團隊/長期維護友善）。
──────────────────────────────────────────────────────────────────────────────
[常見追問（你可口述）]
Q1: 什麼時候該用值傳遞、參考、指標？
A1: 小標量值傳遞；只讀大物件用 `const T&`；可空/可選用指標；需要轉移所有權用 `T&&`（move）。

Q2: RAII 對嵌入式有什麼好處？
A2: 無 GC；釋放時間可預測，不阻塞中斷或造成尾延遲；確保關閉檔案/釋放 DMA buffer/解鎖 mutex。

Q3: `const` 幫助是什麼？
A3: 形成不變性契約，降低 side effect；編譯器可更積極最佳化；接口更容易使用無誤。

Q4: iostream 加速的 trade-off？
A4: 關閉與 C stdio 同步後，不要混用 printf/fprintf 以免 flush 順序難以推論。
──────────────────────────────────────────────────────────────────────────────
[陷阱備忘]
• 參考 (T&) 不能為空；需要「可空」語義請用指標 (T*) 或 std::optional<T*>（或介面改為傳回狀態碼）。
• 大物件值傳遞=昂貴複製；若不需修改，用 `const T&`；若要搬移，考慮 `std::move`.
• iostream 與 stdio 混用需小心 flush 次序；關閉同步後盡量單一陣營。
──────────────────────────────────────────────────────────────────────────────
*/

/// 值傳遞：int 等小型標量複製成本低，語意最直觀
int add_by_value(int a, int b) {
    // 回傳 by value：現代編譯器有 NRVO/移動最佳化，通常零額外成本
    return a + b;
}

/// 參考傳遞：呼叫端變數 a 會被「就地」修改
void add_in_place(int& a, int b) {
    a += b;
}

/// 只讀大型物件：以 const 參考避免複製（示意）
void print_const_ref(const std::string& s) {
    std::cout << "[read-only] " << s << "\n";
}

int main() {
    // ── I/O 優化：嵌入式常見效能習慣 ─────────────────────────────
    // sync_with_stdio(false): 關閉與 C stdio 的同步 → iostream 更快
    // cin.tie(nullptr): 解除 cin 與 cout 綁定 → 避免每次輸入前自動 flush
    // 注意：關閉同步後請避免混用 printf/fprintf。
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    // ── 值傳遞：不影響呼叫端 ───────────────────────────────────
    int x = 3, y = 5;
    std::cout << "add_by_value(x,y): " << add_by_value(x, y) << "\n";
    std::cout << "x still 3? " << x << "  // 值傳遞不改變呼叫端\n";

    // ── 參考傳遞：就地修改呼叫端變數 ────────────────────────────
    add_in_place(x, y);
    std::cout << "after add_in_place(x,y), x = " << x << "  // 被就地修改\n";

    // ── const：唯讀契約（類比 Java final），易於最佳化與防呆 ─────────
    const int z = 42;
    std::cout << "const z = " << z << "\n";

    // ── RAII：std::string 進入 scope 建構、離開 scope 自動釋放（無需手動 free） ─
    std::string s = "hello";
    std::cout << s << "\n";

    // 只讀大型物件以 const& 傳入，避免複製（在 embedded 上降低峰值記憶體/延遲）
    print_const_ref(s);

    /*
    ───────────────────────────────────────────────────────────────────────────
    [延伸 Follow-up（可現場再說或加碼示範）]
    • 若要建模「可空參數」：使用指標 (T*) 或 std::optional<T*>，並清楚文件化所有權。
    • 若要建模「轉移所有權」：以 `T&&` + `std::move` 接口，或使用 RAII wrapper（如 FileHandle）。
    • 若要避免 iostream：在極端資源環境可改用 printf/puts 或裸串列埠輸出，並說明取捨。
    • 效能量測：展示以 -O2/-O3 編譯，或以簡單計時器（std::chrono）量測 I/O 差異。
    ───────────────────────────────────────────────────────────────────────────
    */

    return 0;
}
