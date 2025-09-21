#include <cstddef>
#include <cstdint>
using namespace std;
// TASK: Implement set_bits, clear_bits, write_masked for 32-bit MMIO registers.
// Use volatile and 4-byte offsets.
using u32=uint32_t;
volatile u32* const REG_BASE = (volatile u32*)0x40000000;
inline volatile u32* reg(ptrdiff_t off){ return (volatile u32*)((uintptr_t)REG_BASE + off); }
void set_bits(ptrdiff_t off, u32 mask){ /* TODO */ }
void clear_bits(ptrdiff_t off, u32 mask){ /* TODO */ }
void write_masked(ptrdiff_t off, u32 mask, u32 value){ /* TODO: modify only masked bits */ }
int main(){ return 0; }


// #include <cstddef>
// #include <cstdint>
//
// // ────────────────────────────────────────────────────────────────
// // 32-bit MMIO 基本工具：set_bits / clear_bits / write_masked
// // - volatile：避免最佳化刪除/重排 MMIO 存取（不保證原子/跨核心順序）
// // - 4-byte 對齊：對 32-bit 寄存器正確存取寬度與對齊，避免 bus fault
// // - RMW（read-modify-write）流程：必要時在外層包 IRQ 關閉或用硬體 SET/CLR
// // - write_masked(new = (old & ~mask) | (value & mask))：只改被 mask 指定的位元
// // ────────────────────────────────────────────────────────────────
//
// using u32 = std::uint32_t;
// using uptr = std::uintptr_t;
//
// // 硬體 base（實機依 datasheet 調整）
// volatile u32* const REG_BASE = reinterpret_cast<volatile u32*>(0x40000000);
//
// // 取得偏移對應的 32-bit 暫存器指標（off 必須為 4 的倍數）
// inline volatile u32* reg(std::ptrdiff_t off) {
//     // 可選：在開發版加上斷言（量產可移除）
//     // 這裡不用 assert 避免引入 libc；若需要可自定 ASSERT 宏
//     // if ((off & 0x3) != 0) fault(); // misaligned offset
//     return reinterpret_cast<volatile u32*>(reinterpret_cast<uptr>(REG_BASE) + static_cast<uptr>(off));
// }
//
// // 可選：平台記憶體屏障（實機請改成 DMB/DSB/fence；此處是 compiler barrier）
// inline void dmb() { asm volatile("" ::: "memory"); }
//
// // 設定位元：reg |= mask
// inline void set_bits(std::ptrdiff_t off, u32 mask) {
//     volatile u32* r = reg(off);
//     u32 v = *r;          // 讀
//     v |= mask;           // 改
//     *r = v;              // 寫
//     dmb();
//     // 面試可講：這是 RMW，若有 IRQ/多核心競爭，外層要關中斷或用硬體的 SET 寄存器。
// }
//
// // 清除位元：reg &= ~mask
// inline void clear_bits(std::ptrdiff_t off, u32 mask) {
//     volatile u32* r = reg(off);
//     u32 v = *r;          // 讀
//     v &= ~mask;          // 改
//     *r = v;              // 寫
//     dmb();
//     // 面試可講：若硬體旗標是 W1C（write-1-to-clear），不能用 RMW，應直接寫 1 到對應位。
// }
//
// // 只修改被 mask 指定的位元：new = (old & ~mask) | (value & mask)
// inline void write_masked(std::ptrdiff_t off, u32 mask, u32 value) {
//     volatile u32* r = reg(off);
//     u32 oldv = *r;                         // 讀
//     u32 newv = (oldv & ~mask) | (value & mask);
//     *r = newv;                             // 寫
//     dmb();
//     // 面試可講：
//     // - 僅變更 mask 範圍內的位元，其餘保持不動。
//     // - 一樣是 RMW，注意競爭條件；若有 CTRL_SET/CTRL_CLR 別名暫存器，優先使用它們避免 RMW。
// }
//
// // 範例：不做任何實際讀寫（REG_BASE 是實機位址）
// int main() {
//     // 假設有個控制寄存器 CTRL at offset 0x04
//     // set_bits(0x04, (1u<<0));                 // 使能
//     // clear_bits(0x04, (1u<<1));               // 關 IRQ
//     // write_masked(0x04, 0x0000FF00u, 0x00005500u); // 只改 bits[15:8]
//     return 0;
// }
//
// /*
// ──────────────────────────────────────────────────────────────────────────────
// [面試口條（可直接講）]
// • 我用 volatile + 正確寬度/對齊存取 32-bit MMIO。
// • set_bits/clear_bits 都是 RMW；若有競爭（ISR/多核/硬體自清位），外層需關中斷或用 SET/CLR 別名暫存器。
// • write_masked 只改 mask 指定位元：new = (old & ~mask) | (value & mask)。
// • 若遇到 W1C/W0C/自清位語義，不能用 RMW，要依 datasheet 用正確寫法（例如直接寫 1 清除）。
// • 在需要順序保證時加 barrier（ARM: DMB/DSB/ISB；RISC-V: fence）。
// ──────────────────────────────────────────────────────────────────────────────
// */
