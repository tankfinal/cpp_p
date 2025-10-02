// =================================================================
// 題目八: 單詞搜尋 II (Word Search II)
// =================================================================
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

/*
[題目描述]
給定一個 m x n 的字元網格 `board` 和一個單詞列表 `words`，找出所有在網格中存在的單詞。
單詞可以由相鄰（上、下、左、右）的字元連接而成，同一個位置的字元不能重複使用。

[思路]
這是一個典型的「字典樹 (Trie) + 深度優先搜尋 (DFS) 回溯」問題。直接對每個單詞在網格中搜尋會非常慢。
1.  **建立字典樹 (Trie)**:
    - 將 `words` 列表中的所有單詞存入一個 Trie。
    - Trie 的每個節點可以增加一個 `word` 變數，當一個單詞結束時，將該單詞存儲在此節點中。
2.  **深度優先搜尋 (DFS)**:
    - 遍歷網格中的每一個儲存格，以其作為 DFS 的起點。
    - DFS 函式需要攜帶當前在 Trie 中的節點指標。
    - 在 DFS 的每一步，檢查當前字元是否在 Trie 節點的子節點中。
      - 如果是，則繼續向該子節點遞迴搜尋相鄰的格子。
      - 如果不是，則此路徑無效，剪枝返回。
    - 如果走到的 Trie 節點包含一個完整的單詞（`word` 變數不為空），則表示找到了一個單詞，將其加入結果集。為了避免重複找到，找到後可以將該節點的 `word` 設為空字串。
3.  **回溯 (Backtracking)**:
    - 為了防止在一次 DFS 中重複使用同一個儲存格，我們在訪問一個格子時，可以將其字元暫時替換為一個特殊符號（如 '#'）。
    - 在從該格子的所有相鄰方向遞迴返回後，必須將其字元還原，以便其他的 DFS 路徑可以使用它。這就是回溯。
*/

// 解答 (Solution)
class TrieNode {
public:
    std::unordered_map<char, TrieNode*> children;
    std::string word;

    TrieNode() : word("") {}

    ~TrieNode() {
        for (auto& pair : children) {
            delete pair.second;
        }
    }
};

class Solution {
private:
    int m = 0, n = 0;
    std::vector<std::string> result;

    void dfs(std::vector<std::vector<char>>& board, int i, int j, TrieNode* node) {
        if (i < 0 || i >= m || j < 0 || j >= n || board[i][j] == '#') {
            return;
        }

        char c = board[i][j];
        if (node->children.find(c) == node->children.end()) {
            return;
        }

        node = node->children[c];
        if (!node->word.empty()) {
            result.push_back(node->word);
            node->word = ""; // 避免重複加入
        }

        board[i][j] = '#'; // 標記已訪問

        int di[] = {0, 0, 1, -1};
        int dj[] = {1, -1, 0, 0};
        for (int k = 0; k < 4; ++k) {
            dfs(board, i + di[k], j + dj[k], node);
        }

        board[i][j] = c; // 回溯，還原字元
    }

public:
    std::vector<std::string> findWords(std::vector<std::vector<char>>& board, std::vector<std::string>& words) {
        TrieNode root;
        // 建立 Trie
        for (const std::string& word : words) {
            TrieNode* node = &root;
            for (char c : word) {
                if (node->children.find(c) == node->children.end()) {
                    node->children[c] = new TrieNode();
                }
                node = node->children[c];
            }
            node->word = word;
        }

        m = board.size();
        if (m == 0) return {};
        n = board[0].size();

        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                dfs(board, i, j, &root);
            }
        }

        return result;
    }
};


// main 函式用於測試
int main() {
    std::cout << "--- Testing Word Search II ---" << std::endl;

    std::vector<std::vector<char>> board = {
        {'o', 'a', 'a', 'n'},
        {'e', 't', 'a', 'e'},
        {'i', 'h', 'k', 'r'},
        {'i', 'f', 'l', 'v'}
    };

    std::vector<std::string> words = {"oath", "pea", "eat", "rain"};

    Solution sol;
    std::vector<std::string> found_words = sol.findWords(board, words);

    std::cout << "Board:" << std::endl;
    for(const auto& row : board) {
        for(char c : row) std::cout << c << " ";
        std::cout << std::endl;
    }

    std::cout << "\nWords to find: oath, pea, eat, rain" << std::endl;
    std::cout << "Found words: ";
    for (const std::string& word : found_words) {
        std::cout << word << " ";
    }
    std::cout << "\n(Expected: eat oath or oath eat)" << std::endl;

    std::cout << "--- Test Ended ---" << std::endl;
    return 0;
}