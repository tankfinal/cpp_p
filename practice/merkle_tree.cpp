// =================================================================
// 題目十: 實作默克爾樹 (Merkle Tree) 以驗證資料完整性
// =================================================================
#include <iostream>
#include <vector>
#include <string>
#include <functional> // for std::hash

/*
[題目描述]
設計一個 MerkleTree 類別，它可以從一組資料塊 (data blocks) 中建構一棵默克爾樹，並提供一個方法來獲取樹的根雜湊 (root hash)。默克爾樹是一種雜湊樹，常用於驗證大規模資料的完整性。

[思路]
1.  **基礎層 (葉節點)**: 樹的最底層是資料塊的雜湊值。首先，遍歷所有輸入的資料塊，對每個塊計算其雜湊值，形成葉節點層。
2.  **逐層建構**:
    - 從葉節點層開始，兩兩一組，將相鄰兩個節點的雜湊值串接起來，再對這個新字串計算雜湊，從而生成它們的父節點。
    - 重複這個過程，不斷產生上一層的節點，直到只剩下一個節點為止。這個最後的節點就是默克爾根 (Merkle Root)。
3.  **處理奇數節點**: 如果在某一層的節點數量是奇數，通常的作法是將最後一個節點複製一份，與其自身進行雜湊來產生父節點。
4.  **雜湊函式**: 在這個範例中，我們使用 `std::hash` 來模擬一個雜湊函式。在實際的儲存系統中，會使用如 SHA-256 等加密安全雜湊函式。
5.  **實現**:
    - 類別接收一個 `std::vector<std::string>` 作為資料塊。
    - `build_tree` 函式負責實現上述的逐層建構邏輯。
    - `getRootHash` 方法回傳最終計算出的根雜湊。
*/

// 解答 (Solution)
class MerkleTree {
private:
    std::vector<std::string> data_blocks;
    std::string root_hash;

    // 模擬的雜湊函式，將輸入字串轉換為其雜湊值的字串表示
    std::string hash_function(const std::string& s) {
        return std::to_string(std::hash<std::string>{}(s));
    }

    void build_tree() {
        if (data_blocks.empty()) {
            root_hash = "";
            return;
        }

        std::vector<std::string> current_level_hashes;
        // 計算葉節點層的雜湊
        for (const auto& block : data_blocks) {
            current_level_hashes.push_back(hash_function(block));
        }

        // 如果只有一個資料塊，其雜湊就是根雜湊
        if (current_level_hashes.size() == 1) {
            root_hash = current_level_hashes[0];
            return;
        }

        // 逐層向上建構
        while (current_level_hashes.size() > 1) {
            std::vector<std::string> next_level_hashes;
            for (size_t i = 0; i < current_level_hashes.size(); i += 2) {
                if (i + 1 < current_level_hashes.size()) {
                    // 將相鄰兩個雜湊串接後再雜湊
                    next_level_hashes.push_back(hash_function(current_level_hashes[i] + current_level_hashes[i+1]));
                } else {
                    // 處理奇數節點的情況
                    next_level_hashes.push_back(hash_function(current_level_hashes[i] + current_level_hashes[i]));
                }
            }
            current_level_hashes = next_level_hashes;
        }

        root_hash = current_level_hashes[0];
    }

public:
    MerkleTree(const std::vector<std::string>& blocks) : data_blocks(blocks) {
        build_tree();
    }

    std::string getRootHash() const {
        return root_hash;
    }

    // 驗證函式：比較當前樹的根雜湊與另一組資料產生的根雜湊是否相同
    bool verify(const std::vector<std::string>& new_blocks) const {
        MerkleTree new_tree(new_blocks);
        return new_tree.getRootHash() == this->root_hash;
    }
};

// main 函式用於測試
int main() {
    std::cout << "--- Testing Merkle Tree ---" << std::endl;

    // 情況一：原始資料
    std::vector<std::string> blocks1 = {"blockA", "blockB", "blockC", "blockD"};
    MerkleTree tree1(blocks1);
    std::cout << "Original data blocks: {A, B, C, D}" << std::endl;
    std::cout << "Original Root Hash: " << tree1.getRootHash() << std::endl;

    // 情況二：資料被竄改
    std::vector<std::string> blocks2 = {"blockA", "blockB_modified", "blockC", "blockD"};
    MerkleTree tree2(blocks2);
    std::cout << "\nModified data blocks: {A, B_modified, C, D}" << std::endl;
    std::cout << "Modified Root Hash: " << tree2.getRootHash() << std::endl;

    // 情況三：資料順序改變
    std::vector<std::string> blocks3 = {"blockD", "blockB", "blockC", "blockA"};
    MerkleTree tree3(blocks3);
    std::cout << "\nReordered data blocks: {D, B, C, A}" << std::endl;
    std::cout << "Reordered Root Hash: " << tree3.getRootHash() << std::endl;

    std::cout << "\n--- Verification ---" << std::endl;
    std::cout << "Verify original data against original tree: " << (tree1.verify(blocks1) ? "Success" : "Failed") << " (Expected: Success)" << std::endl;
    std::cout << "Verify modified data against original tree: " << (tree1.verify(blocks2) ? "Success" : "Failed") << " (Expected: Failed)" << std::endl;

    std::cout << "--- Test Ended ---" << std::endl;
    return 0;
}