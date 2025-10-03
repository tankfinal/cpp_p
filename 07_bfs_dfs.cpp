#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <stack>

// =================================================================
// 範例：BFS (使用 Queue) 與 DFS (使用 Stack) 的基礎實作
// =================================================================

// 簡單的樹節點定義
struct TreeNode {
    std::string val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(std::string x) : val(x), left(nullptr), right(nullptr) {}
};

/**
 * @brief 使用 Queue 實現廣度優先搜尋 (BFS)
 * 特點：先進先出，一層一層地遍歷
 */
void bfs_traverse(TreeNode* root) {
    if (!root) {
        return;
    }

    std::queue<TreeNode*> q;
    q.push(root);

    while (!q.empty()) {
        TreeNode* current = q.front();
        q.pop();

        // 訪問 (印出) 節點
        std::cout << current->val << " ";

        // 先將左子節點加入佇列
        if (current->left) {
            q.push(current->left);
        }
        // 再將右子節點加入佇列
        if (current->right) {
            q.push(current->right);
        }
    }
}

/**
 * @brief 使用 Stack 實現深度優先搜尋 (DFS) - 前序遍歷
 * 特點：後進先出，一條路走到底
 */
void dfs_traverse(TreeNode* root) {
    if (!root) {
        return;
    }

    std::stack<TreeNode*> s;
    s.push(root);

    while (!s.empty()) {
        TreeNode* current = s.top();
        s.pop();

        // 訪問 (印出) 節點
        std::cout << current->val << " ";

        // 注意！與 Queue 不同，要先將右子節點推入堆疊
        // 因為堆疊是後進先出，我們先放右邊，再放左邊，
        // 這樣下次取出的 top() 就會是左子節點，實現了「根->左->右」的順序
        if (current->right) {
            s.push(current->right);
        }
        if (current->left) {
            s.push(current->left);
        }
    }
}

// --- main 函式用於建立樹並測試 ---
int main() {
    // 建立一棵樹:
    //      A
    //     / \
    //    B   C
    //   / \   \
    //  D   E   F
    TreeNode* root = new TreeNode("A");
    root->left = new TreeNode("B");
    root->right = new TreeNode("C");
    root->left->left = new TreeNode("D");
    root->left->right = new TreeNode("E");
    root->right->right = new TreeNode("F");

    std::cout << "--- BFS vs DFS Traversal ---" << std::endl;

    std::cout << "BFS traversal (layer by layer, using Queue): ";
    bfs_traverse(root);
    std::cout << std::endl;

    std::cout << "DFS traversal (depth first, using Stack):  ";
    dfs_traverse(root);
    std::cout << std::endl;

    std::cout << "--- Test Ended ---" << std::endl;

    // 在真實程式中，需要手動刪除 new 出來的節點以避免記憶體洩漏
    // 或使用智能指標 (smart pointers) 自動管理
    delete root->left->left;
    delete root->left->right;
    delete root->right->right;
    delete root->left;
    delete root->right;
    delete root;

    return 0;
}