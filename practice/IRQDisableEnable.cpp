using namespace std;
// TASK: Model critical section: irq_disable(); critical register sequence; irq_enable().
// Here we just simulate with a boolean flag and assert no interleaving.
static bool irq_enabled=true;
void irq_disable(){ irq_enabled=false; }
void irq_enable(){ irq_enabled=true; }
int main(){
    // TODO: simulate a race and show how disabling prevents inconsistent state
    return 0;
}


// #include <iostream>
// #include <cassert>
// using namespace std;
//
// // ────────────────────────────────────────────────
// // 模擬 irq 開關：在臨界區內禁止「中斷」打斷
// // ────────────────────────────────────────────────
// static bool irq_enabled = true;
// void irq_disable() { irq_enabled = false; }
// void irq_enable()  { irq_enabled = true;  }
//
// // ────────────────────────────────────────────────
// // 模擬共享狀態：假設我們要一次更新 (regA, regB)，必須成對一致
// // 若在更新中途被中斷 → 狀態不一致
// // ────────────────────────────────────────────────
// struct Device {
//     int regA = 0;
//     int regB = 0;
//
//     // 沒有保護：可能在中途被打斷
//     void update_unprotected(int a, int b) {
//         regA = a;
//         // ❌ 模擬：此時若 IRQ 發生，看到 regA 已更新、regB 尚未更新 → 不一致
//         if (irq_enabled) {
//             cout << "[IRQ] inconsistent state: regA=" << regA
//                  << " regB=" << regB << "\n";
//         }
//         regB = b;
//     }
//
//     // 使用 critical section：disable IRQ 包住
//     void update_protected(int a, int b) {
//         irq_disable();
//         regA = a;
//         regB = b;
//         irq_enable();
//     }
// };
//
// int main() {
//     Device d;
//
//     cout << "=== 模擬未保護的更新 ===\n";
//     d.update_unprotected(1, 99);
//
//     cout << "=== 模擬有保護的更新 ===\n";
//     d.update_protected(2, 88);
//     cout << "consistent state: regA=" << d.regA
//          << " regB=" << d.regB << "\n";
//
//     /*
//     [面試口條]
//     • 臨界區更新需要原子性，否則中斷可能觀察到不一致狀態。
//     • 用 irq_disable()/irq_enable() 可以確保這段更新不被打斷。
//     • 真實硬體上，這通常用來保護多暫存器 sequence，或在無法用 mutex 的 ISR 環境。
//     */
//     return 0;
// }
