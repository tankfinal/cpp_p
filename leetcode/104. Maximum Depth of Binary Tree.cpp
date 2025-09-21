/*
* 題目：LeetCode 104. Maximum Depth of Binary Tree
 * 連結：https://leetcode.com/problems/maximum-depth-of-binary-tree/
 *
 * 【Embedded 對應】
 * - DFS 遞迴：回傳左右子樹最大深度 + 1；簡潔清楚。
 *
 * // LeetCode 既有定義：
 * // struct TreeNode { int val; TreeNode *left; TreeNode *right; TreeNode(): val(0), left(nullptr), right(nullptr) {} TreeNode(int x): val(x), left(nullptr), right(nullptr) {} TreeNode(int x, TreeNode* l, TreeNode* r): val(x), left(l), right(r) {} };
 */

struct TreeNode { int val; TreeNode *left, *right; TreeNode(int x): val(x), left(nullptr), right(nullptr) {} };

class Solution_104 {
public:
    int maxDepth(TreeNode* root) {
        if (!root) return 0;
        int L = maxDepth(root->left), R = maxDepth(root->right);
        return 1 + (L > R ? L : R);
    }
};
