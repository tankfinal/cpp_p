#include <iostream>
#include <cstddef>   // offsetof
#include <cstdint>   // uint8_t, uint16_t, uint32_t

/*
──────────────────────────────────────────────────────────────────────────────
[面試前備忘錄｜Google Embedded 視角]
• offsetof(T, member)：回傳 struct 成員的位元組偏移量（由編譯器計算）。
• 編譯器會插入 padding 來滿足 alignment（對齊需求）。
  - x86 通常允許非對齊存取，但效能差。
  - ARM Cortex-M/A 通常要求對齊，不然 bus fault。
• sizeof(struct) ≥ 最大成員對齊需求，並且整個 struct 會以最大 alignment 對齊。
• 通訊協定封包常要求「無 padding」，要加 `#pragma pack` 或 `__attribute__((packed))`。
• Endianness：在跨平台傳輸數據時，struct memory layout + endian 都會影響結果。

──────────────────────────────────────────────────────────────────────────────
[常見追問（口條提示）]
Q1: 為什麼 len 偏移是 4 而不是 1？
A1: 因為編譯器會插入 3 byte padding，讓 len 對齊到 4-byte 邊界。

Q2: 為什麼 sizeof(Packet) 可能是 12 而不是 7？
A2: 最大成員是 uint32_t，需要 4-byte 對齊；編譯器在最後也會 pad 到 4 的倍數。

Q3: 如何在通訊協定中避免 padding？
A3: 用 `#pragma pack(push,1)` / `#pragma pack(pop)` 或 `__attribute__((packed))`；
    或直接手動 serialize (shift & mask)。

Q4: 為什麼 offsetof 是安全的？
A4: 因為它由編譯器在編譯期計算，不是用 pointer cast hack。

Q5: 在嵌入式裡要注意什麼？
A5:
   • 結構體對齊 → 確保不會 unaligned access bus fault。
   • 協定/寄存器 → 要明確 pack，否則跨平台會錯。
   • Endianness → 小端/大端要轉換（ntohl/htonl）。

──────────────────────────────────────────────────────────────────────────────
[陷阱備忘]
• 千萬不要假設 struct memory layout 在所有編譯器/平台都一樣。
• padding 會導致 `memcpy`/`send` struct 時，對端解析錯誤。
• sizeof(struct) ≥ sum(sizeof(member))，因為有 padding。
• packed struct 可能導致非對齊存取 → 慢甚至 crash（ARM bus fault）。
──────────────────────────────────────────────────────────────────────────────
*/

struct Packet {
    uint8_t  type;   // 1 byte
    // ⚠️ 編譯器會插入 3 bytes padding，讓下個成員對齊到 4-byte
    uint32_t len;    // 4 bytes
    uint16_t flags;  // 2 bytes
    // ⚠️ 編譯器可能在最後再 pad 2 bytes，讓整體大小是 4 的倍數
};

int main() {
    std::cout << "offset(type)  = " << offsetof(Packet, type)  << "\n";
    std::cout << "offset(len)   = " << offsetof(Packet, len)   << "\n";
    std::cout << "offset(flags) = " << offsetof(Packet, flags) << "\n";
    std::cout << "sizeof(Packet)= " << sizeof(Packet) << "\n";

    /*
    ───────────────────────────────────────────────────────────
    [總結口條（可直接講）]
    • Packet 排列：type(1) + padding(3) + len(4) + flags(2) + padding(2) = 12 bytes。
    • 對齊是為了效能 & 硬體需求；否則會 unaligned access。
    • 協定傳輸要避免 padding，建議手動 serialize 或用 packed struct。
    • 在 embedded/網路協定要同時處理 alignment 與 endianness。
    ───────────────────────────────────────────────────────────
    */

    return 0;
}
