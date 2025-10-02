// =================================================================
// 題目二: 序列化與反序列化二元樹 (Serialize and Deserialize Binary Tree)
// =================================================================
#include <iostream>
#include <string>
#include <sstream>
#include <queue>

/*
[題目描述]
設計一個演算法，將一棵二元樹轉換為一個字串（序列化），並能將此字串轉換回原本的二元樹（反序列化）。

[思路]
最直覺且常用的方法是使用前序遍歷 (Pre-order Traversal)。
1.  **序列化**:
    - 從根節點開始進行前序遍歷。
    - 遇到一個節點，就將其值轉換為字串並加入結果中。
    - 如果遇到一個空指標 (nullptr)，就用一個特殊符號（例如 "N" 或 "#"）來代表。
    - 節點值之間用空格或逗號分隔，以利於之後的解析。
2.  **反序列化**:
    - 將序列化後的字串用 `stringstream` 來按分隔符號讀取。
    - 建立一個遞迴輔助函式，每次從 stream 中讀取一個值。
    - 如果讀到的值是特殊符號 "N"，則回傳 nullptr。
    - 否則，用讀到的值建立一個新的 `TreeNode`。
    - 接著，遞迴地呼叫此函式來建立其左子樹和右子樹。
    - 由於序列化是前序，反序列化時也必須按照「根-左-右」的順序來建構。
*/

// 解答 (Solution)
struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

class Codec {
public:
    std::string serialize(TreeNode* root) {
        std::ostringstream out;
        serializeHelper(root, out);
        return out.str();
    }

    TreeNode* deserialize(std::string data) {
        std::istringstream in(data);
        return deserializeHelper(in);
    }

private:
    void serializeHelper(TreeNode* node, std::ostringstream& out) {
        if (!node) {
            out << "N ";
            return;
        }
        out << node->val << " ";
        serializeHelper(node->left, out);
        serializeHelper(node->right, out);
    }

    TreeNode* deserializeHelper(std::istringstream& in) {
        std::string val;
        in >> val;
        if (val == "N") {
            return nullptr;
        }
        TreeNode* node = new TreeNode(std::stoi(val));
        node->left = deserializeHelper(in);
        node->right = deserializeHelper(in);
        return node;
    }
};

// 輔助函式：層序遍歷印出樹來驗證
void printTree(TreeNode* root) {
    if (!root) {
        std::cout << "[empty]" << std::endl;
        return;
    }
    std::queue<TreeNode*> q;
    q.push(root);
    while (!q.empty()) {
        TreeNode* node = q.front();
        q.pop();
        if (node) {
            std::cout << node->val << " ";
            q.push(node->left);
            q.push(node->right);
        } else {
            std::cout << "N ";
        }
    }
    std::cout << std::endl;
}

// main 函式用於測試
int main() {
    std::cout << "--- Testing Serialize/Deserialize Binary Tree ---" << std::endl;

    // 建立一棵樹:
    //      1
    //     / \
    //    2   3
    //       / \
    //      4   5
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->right = new TreeNode(3);
    root->right->left = new TreeNode(4);
    root->right->right = new TreeNode(5);

    std::cout << "Original tree (level order): ";
    printTree(root);

    Codec codec;
    std::string serialized_data = codec.serialize(root);
    std::cout << "Serialized data: " << serialized_data << std::endl;

    TreeNode* deserialized_root = codec.deserialize(serialized_data);
    std::cout << "Deserialized tree (level order): ";
    printTree(deserialized_root);

    std::cout << "--- Test Ended ---" << std::endl;

    // 記得釋放記憶體 (在實際應用中應使用智能指標)
    // ...

    return 0;
}