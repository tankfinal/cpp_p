#include <iostream>
#include <cstdint>
#include <cstdio>

/*
──────────────────────────────────────────────────────────────────────────────
[面試前備忘錄｜Google Embedded 視角]
• BSP (Board Support Package) = 板級支援包：
  - 封裝與特定硬體平台相關的驅動 (GPIO / UART / TIMER / I2C ...)。
  - 上層 App / HAL (Hardware Abstraction Layer) 只呼叫 BSP API，不碰 MMIO 細節。
  - 可攜性：換板子/SoC 只需換 BSP 實作，不影響上層程式。

• 設計分層：
  - Driver / BSP：跟硬體綁定（暫存器、IRQ）。
  - HAL：更抽象的 API，跨 SoC/OS 可共用。
  - App：只面向 HAL/BSP API，不知道底層寄存器。

• 測試策略：
  - 在 PC 上可 mock BSP → 用 fake_ticks 或 stdout 模擬硬體。
  - 在 target MCU 上 → BSP 實作會存取 MMIO 或呼叫 RTOS HAL。
──────────────────────────────────────────────────────────────────────────────
[常見追問（口條提示）]
Q1: 為什麼需要 BSP？
A1: 把硬體依賴隔離；移植板子或換 SoC 時，上層 App 不需改，只換 BSP。

Q2: BSP 與 HAL 的差異？
A2: BSP = 特定硬體驅動；HAL = 抽象 API，跨平台一致。BSP 常由晶片廠提供，HAL 由系統設計者封裝。

Q3: 如何測試 BSP？
A3: 可在 PC 模擬 fake 實作（像這裡的 fake_ticks），或用 dependency injection 注入 mock。

Q4: 在多執行緒 / 多核下如何保證安全？
A4: UART write 要避免同時寫；可用 mutex 或 lock-free ring buffer + ISR 傳輸。

──────────────────────────────────────────────────────────────────────────────
[陷阱備忘]
• BSP 實作若直接呼叫 printf/fputs，在嵌入式上會引入 libc 依賴（不可取）。
• timer_ticks() 應該回傳 **硬體計數器**（不會每呼叫就自增），否則語義錯誤。
• 不要把 BSP 寫死在應用程式 → 要維持 API 層分離。
• ISR 內呼叫阻塞 API（例如 uart_write loop 等待）會卡死 → 要用 non-blocking。
──────────────────────────────────────────────────────────────────────────────
*/

namespace BSP {
    void uart_init(unsigned baud);
    void uart_write(const char* s);
    void timer_start();
    uint32_t timer_ticks();
}

// ---- 範例 BSP 實作（模擬版，可在 PC 執行） ----
namespace BSP {
    static uint32_t fake_ticks = 0;

    void uart_init(unsigned baud) {
        (void)baud; // 實機：設定 UART baud rate 暫存器
    }

    void uart_write(const char* s) {
        std::fputs(s, stdout); // 模擬：輸出到 stdout
    }

    void timer_start() {
        fake_ticks = 0; // 模擬：重置 ticks
    }

    uint32_t timer_ticks() {
        return ++fake_ticks; // 模擬：每呼叫一次自增
        // 實機：會讀取硬體 timer counter（memory-mapped register）
    }
}

// ---- 上層 App 只呼叫 BSP API，不碰硬體細節 ----
int main() {
    BSP::uart_init(115200);
    BSP::timer_start();
    BSP::uart_write("Hello from BSP!\n");

    std::cout << "ticks=" << BSP::timer_ticks() << "\n";
    std::cout << "ticks=" << BSP::timer_ticks() << "\n";

    /*
    ───────────────────────────────────────────────────────────
    [總結口條（可直接講）]
    • BSP 封裝硬體存取；上層只調 BSP API。
    • 這樣移植新板子只需換 BSP 實作，不改應用程式。
    • 我在 PC 用 fake 實作模擬；在 target MCU 就會存取 MMIO。
    • 這就是典型的「分層設計」：App ↔ HAL ↔ BSP ↔ Hardware。
    ───────────────────────────────────────────────────────────
    */
    return 0;
}
