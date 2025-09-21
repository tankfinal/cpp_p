#include <cstdio>
#include <iostream>
#include <array>
#include <cerrno>
#include <cstring>

/*
──────────────────────────────────────────────────────────────────────────────
[面試前備忘錄｜Google Embedded 視角]
• RAII = 建構子取得資源、解構子釋放資源 → 離開 scope 立即釋放（deterministic）。
  - 嵌入式沒有 GC；決定性釋放可避免阻塞中斷/尾延遲（e.g., 關檔、釋放 DMA、解鎖 mutex）。
• 禁止拷貝 (=delete) + 允許移動 (move) → 確保唯一擁有權、可安全轉移資源；避免 double-close。
• 固定大小 buffer（stack 上）優先於 heap：避免碎片與不可預測延遲；可用 std::array。
• errno / perror：檢查 I/O 錯誤（權限、路徑、裝置忙）；面試時主動提「有檢查錯誤」加分。
• 文字檔 vs 二進位：Windows 換行差（"\r\n"）→ 若是 firmware blob 用 "rb"/"wb"。
• 若平台沒有檔案系統：把同樣 RAII 思維搬到 UART/SPI/Flash driver（開→用→關；不得洩漏）。
──────────────────────────────────────────────────────────────────────────────
[常見追問（你可以口述）]
Q1: 為何用 noexcept 在 move ctor/assign？
A1: 承諾「不丟例外」，STL 容器（vector 等）就能放心用 move 最佳化；例外安全更好。

Q2: 如果 fopen 失敗怎麼辦？
A2: 檢查回傳（nullptr），讀取 errno 或用 perror 訊息；不要繼續使用無效的 FILE*。

Q3: 為何選 std::array 做 buffer？
A3: 固定大小、stack 配置、無 heap；在 embedded 環境 deterministic 且低開銷。

Q4: 需要 flush 嗎？
A4: fclose 會 flush；但在故障/斷電敏感情境可先 fflush，再做 fsync(檔案描述符)（POSIX）。

Q5: 若沒有檔案系統？
A5: 類似 RAII 包起來（e.g., UartHandle/SpiFlashHandle），確保 open/close 成對、錯誤可觀察。
──────────────────────────────────────────────────────────────────────────────
[陷阱備忘]
• 別解參考尾後 iterator；別在無效 FILE* 上 fread/fputs。
• 避免忘記關檔（RAII 就是為了杜絕這件事）；避免一個資源被兩個物件關兩次（=delete copy）。
• 注意路徑/權限/工作目錄與平台差異；如需跨平台換行，請明確使用二進位模式 + 自行處理換行。
──────────────────────────────────────────────────────────────────────────────
*/

class FileHandle {
public:
    std::FILE* f;  // C stdio 檔案指標（擁有者）

    // explicit：避免隱式建構；建構子：打開檔案取得資源
    explicit FileHandle(const char* path, const char* mode) : f(std::fopen(path, mode)) {}

    // 解構子：離開 scope 自動關檔（失敗時不丟例外；符合 noexcept 解構子慣例）
    ~FileHandle() { if (f) std::fclose(f); }

    // 禁止複製：確保唯一擁有權，避免 double fclose
    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;

    // 允許移動：轉移擁有權；noexcept 便於 STL 容器做更佳移動最佳化
    FileHandle(FileHandle&& o) noexcept : f(o.f) {
        o.f = nullptr;
    }
    FileHandle& operator=(FileHandle&& o) noexcept {
        if (this != &o) {
            if (f) std::fclose(f);
            f = o.f;
            o.f = nullptr;
        }
        return *this;
    }

    // 便利函式：是否有效（可在 main 中簡化檢查/口述 API 設計）
    explicit operator bool() const noexcept { return f != nullptr; }
};

int main() {
    // 1) 建立/覆寫檔案（示範 RAII + 錯誤處理）
    {
        // 提醒：若是二進位資料請改用 "wb"
        FileHandle out("tmp2.txt", "w");
        if (!out) {
            std::perror("fopen write"); // 顯示 errno 訊息
            return 1;
        }
        if (std::fputs("1234522\n111\n", out.f) < 0) {
            std::perror("fputs");
            return 1;
        }
        if (std::fflush(out.f) != 0) {   // 可選：提早落盤；fclose 也會 flush
            std::perror("fflush");
            return 1;
        }
        // 若需強制同步到儲存（POSIX）：fsync(fileno(out.f));（注意平台可用性）
    } // 離開 scope → 自動 fclose

    // 2) 逐行讀（固定長度 stack buffer；若是二進位請改 fread）
    {
        FileHandle in("tmp2.txt", "r"); // 二進位讀：改 "rb"
        if (!in) {
            std::perror("fopen read");
            return 1;
        }

        std::array<char, 256> buf{}; // 固定大小、無 heap
        while (std::fgets(buf.data(), static_cast<int>(buf.size()), in.f)) {
            std::cout << buf.data(); // fgets 保證 NUL 結尾
        }
        if (std::ferror(in.f)) {      // 讀取循環後檢查錯誤（區分 EOF 與錯誤）
            std::perror("fgets");
            return 1;
        }
    } // 自動 fclose

    // （可選）3) 一次讀完整檔（文字小檔示範；大檔建議流式處理）
    /*
    {
        FileHandle in("tmp2.txt", "rb");
        if (!in) { std::perror("fopen read-all"); return 1; }

        if (std::fseek(in.f, 0, SEEK_END) != 0) { std::perror("fseek"); return 1; }
        long sz = std::ftell(in.f);
        if (sz < 0) { std::perror("ftell"); return 1; }
        std::rewind(in.f);

        std::string content(static_cast<size_t>(sz), '\0');
        size_t n = std::fread(&content[0], 1, content.size(), in.f);
        if (n != content.size()) {
            if (std::ferror(in.f)) { std::perror("fread"); return 1; }
            content.resize(n); // EOF 提早結束
        }
        std::cout << content;
    }
    */

    /*
    ───────────────────────────────────────────────────────────
    [總結口條（可直接說）]
    • 我用 RAII 包裝 FILE*，禁止複製、允許移動，確保唯一擁有權與例外安全。
    • 以固定長度 std::array 當 buffer，避免 heap 碎片與不可預測延遲。
    • 每個 I/O 步驟檢查回傳值並以 perror 診斷原因（權限/路徑/媒體）。
    • 若是二進位資料改 "rb"/"wb"，跨平台避免換行/編碼干擾。
    • 沒有檔案系統時，我會用相同 RAII 思維包 UART/SPI/Flash 介面。
    ───────────────────────────────────────────────────────────
    */

    return 0;
}
