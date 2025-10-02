// =================================================================
// 題目三: 設計一個檔案系統的 find 和 add 功能
// =================================================================
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>

/*
[題目描述]
設計一個類別 FileSystem，模擬一個記憶體內的檔案系統，它支援兩種操作：
1.  add(path): 新增一個檔案路徑。例如 add("/a/b/c")。如果路徑中的目錄 /a 或 /a/b 不存在，需要自動創建。
2.  find(path): 查詢一個檔案路徑是否存在。

[思路]
處理這種階層式路徑結構，最適合的資料結構是字典樹 (Trie)。
1.  **Trie 節點**: 每個 `TrieNode` 代表路徑中的一個部分（目錄或檔案名）。它內部使用一個 `unordered_map<string, TrieNode*>` 來儲存其子目錄/檔案的對應關係。
2.  **路徑分割**: 對於輸入的路徑字串，例如 "/a/b/c"，我們需要先按 '/' 將其分割成 "a", "b", "c" 等部分。`std::stringstream` 和 `std::getline` 是完成這項任務的好工具。
3.  **add 操作**: 從根節點開始，逐一檢查路徑的每個部分。如果某個部分在當前節點的 children map 中不存在，就為它建立一個新的 `TrieNode`。然後沿著路徑繼續向下，直到處理完所有部分。
4.  **find 操作**: 同樣從根節點開始，逐一檢查路徑的每個部分。如果在任何一步發現某部分不存在於 children map 中，就表示路徑不存在，回傳 false。如果成功走完所有部分，表示路徑存在，回傳 true。
5.  **記憶體管理**: 由於 `TrieNode` 是動態配置的 (`new`)，必須在 `FileSystem` 的解構函式中遞迴地釋放所有節點，以避免記憶體洩漏。
*/

// 解答 (Solution)
class TrieNode {
public:
    std::unordered_map<std::string, TrieNode*> children;
    ~TrieNode() {
        for (auto& pair : children) {
            delete pair.second;
        }
    }
};

class FileSystem {
private:
    TrieNode* root;

    // 將路徑分割成字串向量
    std::vector<std::string> splitPath(const std::string& path) {
        std::vector<std::string> parts;
        std::stringstream ss(path);
        std::string part;
        while (std::getline(ss, part, '/')) {
            if (!part.empty()) {
                parts.push_back(part);
            }
        }
        return parts;
    }

public:
    FileSystem() {
        root = new TrieNode();
    }

    ~FileSystem() {
        delete root;
    }

    void add(const std::string& path) {
        auto parts = splitPath(path);
        TrieNode* node = root;
        for (const auto& part : parts) {
            if (node->children.find(part) == node->children.end()) {
                node->children[part] = new TrieNode();
            }
            node = node->children[part];
        }
    }

    bool find(const std::string& path) {
        auto parts = splitPath(path);
        TrieNode* node = root;
        for (const auto& part : parts) {
            if (node->children.find(part) == node->children.end()) {
                return false;
            }
            node = node->children[part];
        }
        return true;
    }
};


// main 函式用於測試
int main() {
    std::cout << "--- Testing File System (Trie) ---" << std::endl;
    FileSystem fs;

    fs.add("/a/b/c");
    std::cout << "Added path: /a/b/c" << std::endl;
    fs.add("/a/d");
    std::cout << "Added path: /a/d" << std::endl;

    std::cout << "Find '/a/b/c': " << (fs.find("/a/b/c") ? "true" : "false") << " (Expected: true)" << std::endl;
    std::cout << "Find '/a/b': " << (fs.find("/a/b") ? "true" : "false") << " (Expected: true)" << std::endl;
    std::cout << "Find '/a/d': " << (fs.find("/a/d") ? "true" : "false") << " (Expected: true)" << std::endl;
    std::cout << "Find '/a': " << (fs.find("/a") ? "true" : "false") << " (Expected: true)" << std::endl;
    std::cout << "Find '/a/e': " << (fs.find("/a/e") ? "true" : "false") << " (Expected: false)" << std::endl;
    std::cout << "Find '/a/b/d': " << (fs.find("/a/b/d") ? "true" : "false") << " (Expected: false)" << std::endl;

    std::cout << "--- Test Ended ---" << std::endl;
    return 0;
}