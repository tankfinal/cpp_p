/*
* 題目：LeetCode 101. Symmetric Tree
 * 連結：https://leetcode.com/problems/symmetric-tree/
 *
 * 【Embedded 對應】
 * - 鏡像判斷：遞迴比較 (L.left,R.right) 與 (L.right,R.left)。
 */

class Solution_101 {
    bool mirror(TreeNode* a, TreeNode* b) {
        if (!a || !b) return a == b;
        return a->val == b->val
            && mirror(a->left,  b->right)
            && mirror(a->right, b->left);
    }
public:
    bool isSymmetric(TreeNode* root) {
        if (!root) return true;
        return mirror(root->left, root->right);
    }
};
