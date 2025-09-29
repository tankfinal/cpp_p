#include <iostream>
#include <cstdint>
#include <iomanip>
#include <string>
using namespace std;

// 小工具：把 32-bit 整數印成二進位，方便觀察（C++11 OK）
string bin32(uint32_t x) {
    string s(32, '0');
    for (int bit = 31; bit >= 0; --bit) {
        if (x & (1u << bit)) s[31 - bit] = '1';
    }
    return s;
}

// -std=c++11 -O2
int main() {
    cout << boolalpha; // 印 bool 為 true/false

    // ------------------------------------------------------------
    // 0. 基本觀念：bit 位置、mask
    // 0x00000016 == 22 十進位；原本的 0b000...010110 改為十六進位
    // ------------------------------------------------------------
    uint32_t x = 0x00000016u;
    cout << "初始 x = " << x << " (bin=" << bin32(x) << ")\n\n";

    // 1) set bit
    {
        uint32_t a = x;
        int k = 5;
        a |= (1u << k);
        cout << "[Set]    設第" << k << "位: " << bin32(a) << "  (十進位 " << a << ")\n";
    }

    // 2) clear bit
    {
        uint32_t a = x;
        int k = 2;
        a &= ~(1u << k);
        cout << "[Clear]  清第" << k << "位: " << bin32(a) << "  (十進位 " << a << ")\n";
    }

    // 3) toggle bit
    {
        uint32_t a = x;
        int k = 1;
        a ^= (1u << k);
        cout << "[Toggle] 反第" << k << "位: " << bin32(a) << "  (十進位 " << a << ")\n";
    }

    // 4) test bit
    {
        int k = 4;
        bool on = (x & (1u << k)) != 0;
        cout << "[Test]   第" << k << "位是否為1? " << on << "\n";
    }

    // 5) 低8位 mask
    {
        uint32_t mask_low8 = (1u << 8) - 1u; // 0xFF
        cout << "[Mask]   低8位 mask: " << bin32(mask_low8) << "  (十進位 " << mask_low8 << ")\n";
        uint32_t low8 = x & mask_low8;
        cout << "         x的低8位: " << bin32(low8) << "  (十進位 " << low8 << ")\n";
    }

    // 6) 取得最低的1位 (isolate lowest set bit)
    {
        uint32_t a = x;                   // 22: ...010110
        uint32_t lowest1 = a & (~a + 1);  // 等價於 a & -a（在二補數系統）
        cout << "[Low1]   取最低1位: " << bin32(lowest1) << "  (十進位 " << lowest1 << ")\n";
    }

    // 7) 清除最低的1位
    {
        uint32_t a = x; // 22
        uint32_t b = a & (a - 1);
        cout << "[Clr1]   清最低1位: from " << bin32(a) << " -> " << bin32(b)
             << "  (十進位 " << b << ")\n";
    }

    // 8) 數1的個數（Brian Kernighan）
    {
        uint32_t a = x;
        int cnt = 0;
        while (a) { a &= (a - 1); ++cnt; }
        cout << "[Popcnt] x共有 " << cnt << " 個1位\n";
    }

    // 9) 位元奇偶校驗（parity）
    {
        uint32_t a = x;
        int parity = 0;
        while (a) { parity ^= 1; a &= (a - 1); }
        cout << "[Parity] x 一的個數是 " << (parity ? "奇數" : "偶數") << "\n";
    }

    // 10) XOR 交換（注意避免同一變數）
    {
        uint32_t a = 5, b = 12; // 0101, 1100
        cout << "[XORswap] before a=" << a << ", b=" << b << "\n";
        a ^= b; b ^= a; a ^= b;
        cout << "           after  a=" << a << ", b=" << b << "\n";
    }

    // 11) 打包/解包（ARGB）
    {
        uint32_t A = 0xFF, R = 0x80, G = 0x40, B = 0x20;
        uint32_t argb = (A << 24) | (R << 16) | (G << 8) | B;
        cout << "[Pack]   ARGB 打包: " << bin32(argb)
             << " (hex 0x" << hex << uppercase << argb << nouppercase << dec << ")\n";

        uint32_t A2 = (argb >> 24) & 0xFF;
        uint32_t R2 = (argb >> 16) & 0xFF;
        uint32_t G2 = (argb >> 8)  & 0xFF;
        uint32_t B2 = (argb)       & 0xFF;
        cout << "[Unpack] A=" << A2 << ", R=" << R2 << ", G=" << G2 << ", B=" << B2 << "\n";
    }

    // 12) 權限旗標（flags）
    {
        const uint32_t READ  = 1u << 0; // 0001
        const uint32_t WRITE = 1u << 1; // 0010
        const uint32_t EXEC  = 1u << 2; // 0100

        uint32_t perm = 0;
        perm |= READ;
        perm |= WRITE;
        cout << "[Flags]  perm=" << bin32(perm) << "  (R?" << bool(perm & READ)
             << ", W?" << bool(perm & WRITE) << ", X?" << bool(perm & EXEC) << ")\n";

        perm |= EXEC;
        cout << "         add X -> " << bin32(perm) << "\n";

        perm &= ~WRITE;
        cout << "         rm  W -> " << bin32(perm) << "  (W?" << bool(perm & WRITE) << ")\n";

        perm ^= READ;
        cout << "         xor R -> " << bin32(perm) << "  (R?" << bool(perm & READ) << ")\n";
    }

    // 13) 範圍欄位（field）擷取/寫入：bits[5..14]（10-bit）
    {
        uint32_t reg = 0;

        // C++11 可用 lambda（OK）
        const uint32_t FIELD_WIDTH = 10;
        const uint32_t FIELD_SHIFT = 5;
        const uint32_t FIELD_MASK  = ((1u << FIELD_WIDTH) - 1u) << FIELD_SHIFT;

        // 寫入：先清掉該區段，再 OR 新值（限制在 10 bit 內）
        // 傳值/傳參都 OK，這裡直接用引用參數
        auto write_field_5_14 = [&](uint32_t& r, uint32_t val) {
            r = (r & ~FIELD_MASK) | ((val & ((1u << FIELD_WIDTH) - 1u)) << FIELD_SHIFT);
        };
        // 讀取
        auto read_field_5_14 = [&](uint32_t r) -> uint32_t {
            return (r >> FIELD_SHIFT) & ((1u << FIELD_WIDTH) - 1u);
        };

        write_field_5_14(reg, 0x155u); // 0b0101010101 = 341
        cout << "[Field]  reg=" << bin32(reg) << ", 讀到=" << read_field_5_14(reg) << "\n";
    }

    cout << "\n全部示範完成qq ✅\n";
    return 0;
}
