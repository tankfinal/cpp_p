using namespace std;
// TASK: Define a packed header with fields (u8 type, u32 len, u16 flags).
// Print offsetof(...) and sizeof(...). Discuss alignment.
struct Header {
    // TODO
};
int main(){
    // TODO: print offsets and sizeof
    return 0;
}

// #include <iostream>
// #include <cstddef>   // offsetof
// #include <cstdint>   // uint8_t, uint16_t, uint32_t
//
// /*
// ──────────────────────────────────────────────────────────────────────────────
// [面試前備忘錄｜Google Embedded 視角]
// • C/C++ struct 預設會依據成員型別 alignment 插入 padding：
//   - uint8_t → 1-byte 對齊
//   - uint16_t → 2-byte 對齊
//   - uint32_t → 4-byte 對齊
// • 編譯器會：
//   1. 在成員之間插 padding，確保每個成員地址符合 alignment。
//   2. 在 struct 尾端補 padding，確保整體大小是最大 alignment 的倍數。
// • offsetof(T, field) 可檢查實際偏移。
// • 若要精確控制 layout（例如通訊協定 header、MMIO register map）→ 用 packed。
// ──────────────────────────────────────────────────────────────────────────────
// [陷阱備忘]
// • 不同編譯器/ABI padding 規則可能略不同 → 不可假設跨平台一致。
// • packed struct 在 ARM 上可能導致 unaligned access → Bus Fault。
// • 一般協定建議「手動序列化」而不是直接傳 struct。
// ──────────────────────────────────────────────────────────────────────────────
// */
//
// // 預設對齊的 Header
// struct Header {
//     uint8_t  type;   // 1 byte
//     uint32_t len;    // 4 bytes，必須對齊到 4 → 前面插入 3 byte padding
//     uint16_t flags;  // 2 bytes
//     // 編譯器可能在尾端再 pad 2 bytes → sizeof(Header) = 12
// };
//
// // 強制 packed，不插 padding（小心非對齊存取）
// #pragma pack(push,1)
// struct PackedHeader {
//     uint8_t  type;
//     uint32_t len;
//     uint16_t flags;
// };
// #pragma pack(pop)
//
// int main() {
//     std::cout << "=== Default aligned Header ===\n";
//     std::cout << "offset(type)  = " << offsetof(Header, type)  << "\n";
//     std::cout << "offset(len)   = " << offsetof(Header, len)   << "\n";
//     std::cout << "offset(flags) = " << offsetof(Header, flags) << "\n";
//     std::cout << "sizeof(Header)= " << sizeof(Header) << "\n\n";
//
//     std::cout << "=== PackedHeader ===\n";
//     std::cout << "offset(type)  = " << offsetof(PackedHeader, type)  << "\n";
//     std::cout << "offset(len)   = " << offsetof(PackedHeader, len)   << "\n";
//     std::cout << "offset(flags) = " << offsetof(PackedHeader, flags) << "\n";
//     std::cout << "sizeof(PackedHeader)= " << sizeof(PackedHeader) << "\n";
//
//     /*
//     ───────────────────────────────────────────────────────────
//     [總結口條（可直接講）]
//     • 預設 Header = 12 bytes (1 + 3 pad + 4 + 2 + 2 pad)。
//     • PackedHeader = 7 bytes，沒有 padding。
//     • 預設對齊 → 存取效能佳、符合硬體要求。
//     • packed → 適合通訊協定/封包，但在 ARM 上可能 unaligned fault。
//     • 實務上：若要跨平台傳輸 → 手動 serialize/deserialize，避免 struct layout 問題。
//     ───────────────────────────────────────────────────────────
//     */
//     return 0;
// }
