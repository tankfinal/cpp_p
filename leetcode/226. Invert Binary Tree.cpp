/*
* 題目：LeetCode 226. Invert Binary Tree
 * 連結：https://leetcode.com/problems/invert-binary-tree/
 *
 * 【Embedded 對應】
 * - 節點左右子樹交換：可用遞迴或迭代（BFS/DFS）實作；經典 easy。
 */

#include <utility>
class Solution_226 {
public:
    TreeNode* invertTree(TreeNode* root) {
        if (!root) return nullptr;
        std::swap(root->left, root->right);
        invertTree(root->left);
        invertTree(root->right);
        return root;
    }
};
