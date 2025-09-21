#include <iostream>
#include <cstdint>
#include <cstddef>

/*
──────────────────────────────────────────────────────────────────────────────
[面試前備忘錄｜Google Embedded 視角]
• MMIO = 將外設暫存器映射到記憶體位址空間；透過讀寫該位址來控制硬體。
• volatile：阻止編譯器刪/重排對該位址的存取（不保證原子性/順序性/可見性跨核心）。
• RMW（read-modify-write）風險：多核心/IRQ/硬體自清位可能導致丟位。必要時用：
  - 臨界區（關中斷/自旋鎖），或
  - 專用原子/別名位址（bit-band/SET/CLR 寄存器）。
• 記憶體順序：有些 CPU/匯流排需要 barrier（ARM DMB/DSB/ISB、RISC-V fence）確保外設看見順序。
• 寬度/對齊：照硬體規格用正確的存取寬度（8/16/32/64）與對齊；錯誤位寬可能觸發 bus fault。
• 結構映射：用 `struct` 對映暫存器要小心 padding/對齊；以 `static_assert` 檢查偏移。
──────────────────────────────────────────────────────────────────────────────
[常見追問（口條提示）]
Q1: 為何用 volatile？它解決什麼、不能解決什麼？
A1: 防最佳化/重排 MMIO 存取；但**不**保證原子性與跨核心順序，需要鎖或 barrier。

Q2: 如何避免 RMW 丟位？
A2: 關中斷包住臨界區、使用硬體提供的 SET/CLR 寄存器、或使用原子別名位址（如 bit-band）。

Q3: 何時需要 memory barrier？
A3: 啟動外設前後、清/設狀態位後、與 DMA/Cache 交互時；在 ARM 用 DMB/DSB/ISB。

Q4: 可以用 std::atomic 嗎？
A4: 原則上**不要**對 MMIO 用 `std::atomic`（語意針對一般記憶體）；MMIO 依硬體規格與 BSP 提供的 barrier 宏。
──────────────────────────────────────────────────────────────────────────────
[陷阱備忘]
• 以錯誤位寬/未對齊的方式存取寄存器。
• 忽略硬體自清位（W1C/RC/RS/C0 等語義）直接做 RMW 導致丟位。
• 以為 volatile 保證多核心/設備間順序（它不會）。
• 用 struct 對映但忘了 `static_assert(offsetof==offset)`，導致錯位。
──────────────────────────────────────────────────────────────────────────────
*/

using u32 = std::uint32_t;

// 寄存器偏移與位元定義（實機：依 datasheet）
// 小抄：集中定義 bits，避免魔數散落
static const std::ptrdiff_t REG_STATUS_OFF = 0x00;
static const std::ptrdiff_t REG_CTRL_OFF   = 0x04;

static const u32 CTRL_ENABLE  = (1u << 0);
static const u32 CTRL_IRQ_EN  = (1u << 1);
static const u32 STATUS_READY = (1u << 0);

// 模擬硬體區域：在 PC 上跑用；實機會用固定位址（例如 0x4000'0000）
// 注意：volatile 表示「每次都真的讀/寫記憶體」，不可快取或刪除
namespace HW {
    volatile u32 REGS[2] = {0, 0}; // REGS[0]=STATUS, REGS[1]=CTRL

    inline volatile u32* reg(std::ptrdiff_t off) {
        std::size_t idx = static_cast<std::size_t>(off / 4);
        return &REGS[idx];
    }

    // 實機可能會是：
    // inline volatile u32* reg(std::ptrdiff_t off) {
    //   return reinterpret_cast<volatile u32*>(0x40000000u + off);
    // }
}

// IRQ 遮罩：示意（實機：由 BSP/HAL 或匯編指令實作，例如 CPSID/CPSIE）
inline void irq_disable() { /* 實機：關中斷 */ }
inline void irq_enable()  { /* 實機：開中斷 */  }

// 可選：平台 barrier（示意）。實機應使用 BSP 提供的 DMB/DSB/ISB/fence 宏。
// inline void dmb() { __asm__ volatile("dmb sy" ::: "memory"); } // ARM 例
inline void dmb() { asm volatile("" ::: "memory"); } // 在 PC 模擬：compiler barrier

// 設備啟用：典型 RMW（read-modify-write）流程，臨界區避免競爭
void device_enable() {
    irq_disable();               // 避免 RMW 與 IRQ 競爭
    volatile u32* ctrl = HW::reg(REG_CTRL_OFF);
    u32 v = *ctrl;               // 讀
    v |= (CTRL_ENABLE | CTRL_IRQ_EN);
    *ctrl = v;                   // 寫
    dmb();                       // 確保寫入對外設可見（實機請換平台 barrier）
    irq_enable();
}

// Poll 裝置是否就緒
bool device_ready() {
    volatile u32* status = HW::reg(REG_STATUS_OFF);
    // 讀之前/之後需要 barrier 與否，依外設時序需求（示意）
    u32 v = *status;
    return (v & STATUS_READY) != 0;
}

int main() {
    device_enable();

    // 模擬：硬體把 READY 位置 1（實機由外設/ISR 設置）
    *HW::reg(REG_STATUS_OFF) |= STATUS_READY;

    std::cout << "Ready? " << (device_ready() ? "yes" : "no") << "\n";
    return 0;
}

/*
──────────────────────────────────────────────────────────────────────────────
[延伸小抄｜實機範式與加分點]
• 寫 1 清除 (W1C)：清中斷旗標時用 *REG = BIT；不要做 RMW：*REG |= BIT 會丟其它 W1C 位。
• SET/CLR 別名寄存器：若有 CTRL_SET/CTRL_CLR，用它們避免 RMW。
• 結構映射：
   struct Regs { volatile u32 STATUS; volatile u32 CTRL; };
   static_assert(offsetof(Regs, CTRL) == 0x04, "CTRL offset mismatch");
   auto* regs = reinterpret_cast<Regs*>(BASE);
• DMA/快取：記得 cache 管理（clean/invalidate）與 barrier；否則資料不同步。
• 多核心：需再加上跨核心同步（鎖/SEV/WFE 等），僅靠 volatile 不足。
──────────────────────────────────────────────────────────────────────────────
*/
