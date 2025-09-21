#include <iostream>
#include <queue>

/*
──────────────────────────────────────────────────────────────────────────────
[面試前備忘錄｜Google Embedded 視角]
• Driver API 設計模式：
  - 平台/廠商提供底層 driver API (start/stop/read/write)。
  - 上層應用只依賴抽象介面 DriverAPI，不直接碰硬體暫存器。
  - MyDriver 是一個實作，模擬 IRQ push 資料 → App 透過 read() 消費。

• 典型分層：
  App (呼叫者) ↔ Driver API (抽象) ↔ Driver Impl (硬體依賴/MMIO/ISR)
──────────────────────────────────────────────────────────────────────────────
[常見追問（口條提示）]
Q1: 為什麼要用抽象 class (DriverAPI)？
A1: 這樣可以換不同硬體實作而不改上層程式。符合「依賴倒置原則」。

Q2: 為什麼 read() 回傳 bool？
A2: 回報是否有資料，避免例外/阻塞；嵌入式 ISR/RT loop 常用這種語義。

Q3: 這樣的 queue 安全嗎？
A3: std::queue 在多執行緒/ISR 下不是安全的；實機上需要 lock-free 或 disable IRQ 保護。

Q4: ISR push 可能被打斷，怎麼處理？
A4:
  - 單核 → push/pop 需保護（短時間關中斷，或用 lock-free ring buffer）。
  - 多核 → 需 atomic 或 memory barrier，避免 race。

Q5: 如果要支援多種 Driver 怎麼辦？
A5: 用工廠模式或注入不同 DriverAPI 實作，App 只依賴抽象。

──────────────────────────────────────────────────────────────────────────────
[陷阱備忘]
• ISR 不能呼叫可能阻塞的 API（例如 std::queue push/pop 有 malloc 風險）。
• std::queue 在嵌入式不是最佳解，應用固定大小 ring buffer（避免 heap）。
• virtual destructor 必須提供，否則 delete base* 會 UB。
──────────────────────────────────────────────────────────────────────────────
*/

class DriverAPI {
public:
    virtual void start() = 0;                  // 啟用 driver（設定暫存器/開 IRQ）
    virtual void stop() = 0;                   // 停用 driver
    virtual bool read(int& out) = 0;           // 嘗試讀取資料；無資料回傳 false
    virtual ~DriverAPI() {}                    // virtual dtor：避免 polymorphic delete UB
};

class MyDriver : public DriverAPI {
    std::queue<int> q;                         // 模擬 FIFO buffer
public:
    void start() override { /* 啟用硬體（這裡略過） */ }
    void stop()  override { /* 停用硬體 */ }

    bool read(int& out) override {
        if (q.empty()) return false;
        out = q.front(); q.pop();
        return true;
    }

    void isr_push(int v){ q.push(v); }         // 模擬 ISR 收到資料並放入 FIFO
};

int main() {
    MyDriver drv;
    drv.start();

    // 模擬 IRQ 產生 3 筆資料
    for (int i=0; i<3; ++i) drv.isr_push(100+i);

    int x;
    while (drv.read(x)) {
        std::cout << "got " << x << "\n";
    }

    drv.stop();

    /*
    ───────────────────────────────────────────────────────────
    [總結口條（可直接講）]
    • DriverAPI 定義抽象介面，上層 App 不關心硬體。
    • MyDriver 模擬 IRQ → FIFO → read() 消費資料。
    • ISR push 在實機上不能用 std::queue（可能 malloc）；要用固定大小 ring buffer。
    • 多核/ISR → 需要 atomic 或 disable IRQ 保護。
    • 設計上符合「抽象介面」+「具體實作」分離，可擴展到多硬體。
    ───────────────────────────────────────────────────────────
    */
    return 0;
}
