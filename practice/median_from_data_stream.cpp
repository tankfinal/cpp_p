// =================================================================
// 題目五: 從資料流中尋找中位數 (Find Median from Data Stream)
// =================================================================
#include <iostream>
#include <queue>
#include <vector>
#include <iomanip> // for std::fixed, std::setprecision

/*
[題目描述]
設計一個資料結構，支援以下兩種操作：
1.  addNum(num): 從資料流中新增一個整數。
2.  findMedian(): 回傳目前所有元素的中位數。

[思路]
關鍵是如何在新增數字後，仍然能快速找到中位數。使用「雙堆 (Two Heaps)」是解決這個問題的經典方法。
1.  **資料結構**: 我們使用兩個優先佇列 (priority_queue)：
    - 一個**最大堆 (Max-Heap)** `small_half`，儲存資料流中數值較小的一半數字。
    - 一個**最小堆 (Min-Heap)** `large_half`，儲存資料流中數值較大的一半數字。
2.  **維持平衡**: 始終保持兩個堆的大小平衡，它們的大小差距不能超過 1。`small_half` 的大小可以等於或比 `large_half` 多一。
3.  **`addNum` 操作**:
    - 將新數字 `num` 先推入最大堆 `small_half`。
    - 為了維持 `small_half` 的所有元素都小於等於 `large_half` 的所有元素，我們將 `small_half` 的頂部元素（最大值）彈出，並推入 `large_half`。
    - 最後，進行大小平衡。如果 `large_half` 的大小超過了 `small_half`，就從 `large_half` 彈出頂部元素（最小值）並推入 `small_half`。
4.  **`findMedian` 操作**:
    - 如果元素總數是奇數，中位數就是 `small_half` 的頂部元素（因為我們讓它多一個）。
    - 如果元素總數是偶數，中位數就是 `small_half` 的頂部和 `large_half` 的頂部的平均值。
*/

// 解答 (Solution)
class MedianFinder {
private:
    std::priority_queue<int, std::vector<int>, std::less<int>> small_half; // Max-heap
    std::priority_queue<int, std::vector<int>, std::greater<int>> large_half; // Min-heap

public:
    MedianFinder() {}

    void addNum(int num) {
        small_half.push(num);

        // 將 small_half 的最大值移到 large_half
        large_half.push(small_half.top());
        small_half.pop();

        // 平衡大小，確保 small_half 的大小 >= large_half 的大小
        if (small_half.size() < large_half.size()) {
            small_half.push(large_half.top());
            large_half.pop();
        }
    }

    double findMedian() {
        if (small_half.size() > large_half.size()) {
            return small_half.top();
        } else {
            if (small_half.empty()) return 0.0;
            return (small_half.top() + large_half.top()) / 2.0;
        }
    }
};

// main 函式用於測試
int main() {
    std::cout << "--- Testing Median from Data Stream ---" << std::endl;
    MedianFinder mf;

    mf.addNum(1);
    std::cout << "addNum(1), Median: " << mf.findMedian() << " (Expected: 1.0)" << std::endl;
    mf.addNum(2);
    std::cout << "addNum(2), Median: " << mf.findMedian() << " (Expected: 1.5)" << std::endl;
    mf.addNum(3);
    std::cout << "addNum(3), Median: " << mf.findMedian() << " (Expected: 2.0)" << std::endl;
    mf.addNum(4);
    std::cout << "addNum(4), Median: " << mf.findMedian() << " (Expected: 2.5)" << std::endl;
    mf.addNum(0);
    std::cout << "addNum(0), Median: " << mf.findMedian() << " (Expected: 2.0)" << std::endl;

    std::cout << std::fixed << std::setprecision(5);
    std::cout << "Final median after adding numbers is: " << mf.findMedian() << std::endl;

    std::cout << "--- Test Ended ---" << std::endl;
    return 0;
}