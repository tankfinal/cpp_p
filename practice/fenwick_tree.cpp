// =================================================================
// 題目九: 區間和查詢 - 樹狀陣列 (Fenwick Tree / BIT)
// =================================================================
#include <iostream>
#include <vector>
#include <numeric> // for std::accumulate

/*
[題目描述]
給定一個整數陣列 nums，高效地處理以下兩種操作：
1.  update(index, val): 將索引為 index 的元素值更新為 val。
2.  sumRange(left, right): 回傳陣列中索引從 left 到 right (包含) 的區間和。

[思路]
這個問題是樹狀陣列 (Fenwick Tree 或 Binary Indexed Tree, BIT) 的經典應用。樹狀陣列是一種資料結構，可以在對數時間內完成「單點更新」和「前綴和查詢」。
1.  **核心思想**: 樹狀陣列 `bit` 的每個位置 `bit[i]` 儲存的是原始陣列中一個特定區間的和。這個區間的長度由 `i` 的二進位表示中最低位的 1 (lowbit) 決定。
2.  **lowbit 操作**: `lowbit(i)` 回傳 `i` 的二進位表示中最低位的 1 所代表的值。在程式碼中，`i & -i` 是一個巧妙的實作。
3.  **`add` 操作**: 當我們要更新原始陣列 `nums[i]` 時，需要在樹狀陣列中更新所有包含 `nums[i]` 的區間。這些區間的索引可以通過 `i = i + lowbit(i)` 來依次找到。
4.  **`query` 操作**: 當我們要查詢前綴和 `sum(0...i)` 時，可以通過 `i = i - lowbit(i)` 來依次累加相關區間的和。
5.  **`sumRange(left, right)`**: 區間和可以通過兩個前綴和相減得到：`query(right) - query(left - 1)`。
6.  **索引**: 樹狀陣列的標準實作通常從索引 1 開始，而 C++ 陣列索引從 0 開始，因此在實作時需要進行 `index + 1` 的轉換。

時間複雜度: `update` 和 `sumRange` 都是 O(log N)。建構整個樹是 O(N log N)。
*/

// 解答 (Solution)
class NumArray {
private:
    std::vector<int> bit;
    std::vector<int> nums_copy;
    int size;

    // 回傳 x 的二進位表示中最低位的 1
    int lowbit(int x) {
        return x & -x;
    }

    // 在樹狀陣列的 index 位置增加 val
    void add(int index, int val) {
        for (int i = index; i <= size; i += lowbit(i)) {
            bit[i] += val;
        }
    }

    // 查詢前綴和 (從 1 到 index)
    int query(int index) {
        int sum = 0;
        for (int i = index; i > 0; i -= lowbit(i)) {
            sum += bit[i];
        }
        return sum;
    }

public:
    NumArray(std::vector<int>& nums) {
        size = nums.size();
        bit.assign(size + 1, 0);
        nums_copy.assign(size, 0);
        for (int i = 0; i < size; ++i) {
            update(i, nums[i]);
        }
    }

    void update(int index, int val) {
        int delta = val - nums_copy[index];
        nums_copy[index] = val;
        add(index + 1, delta); // BIT 索引從 1 開始
    }

    int sumRange(int left, int right) {
        return query(right + 1) - query(left);
    }
};

// main 函式用於測試
int main() {
    std::cout << "--- Testing Fenwick Tree (NumArray) ---" << std::endl;

    std::vector<int> nums = {1, 3, 5};
    NumArray numArray(nums);

    std::cout << "Initial array: {1, 3, 5}" << std::endl;

    // 測試 sumRange
    int sum1 = numArray.sumRange(0, 2);
    std::cout << "sumRange(0, 2): " << sum1 << " (Expected: 9)" << std::endl;

    // 測試 update
    numArray.update(1, 2);
    std::cout << "\nUpdated index 1 to value 2. Array is now conceptually {1, 2, 5}" << std::endl;

    // 再次測試 sumRange
    int sum2 = numArray.sumRange(0, 2);
    std::cout << "sumRange(0, 2): " << sum2 << " (Expected: 8)" << std::endl;

    int sum3 = numArray.sumRange(1, 1);
    std::cout << "sumRange(1, 1): " << sum3 << " (Expected: 2)" << std::endl;

    std::cout << "--- Test Ended ---" << std::endl;
    return 0;
}