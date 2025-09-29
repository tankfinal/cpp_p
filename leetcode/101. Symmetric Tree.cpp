/**
* Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode() : val(0), left(nullptr), right(nullptr) {}
 *     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
 *     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 * };
 */
class Solution {
public:
    // bool isSymmetric(TreeNode* root) {
    //     if (!root) {
    //         return true;
    //     }
    //     return isMirror(root->left, root->right);
    // }

    // bool isMirror(TreeNode* left, TreeNode* right) {
    //     if (!left && !right) {
    //         return true;
    //     }
    //     if (!left || !right) {
    //         return false;
    //     }
    //     if (left->val != right->val) {
    //         return false;
    //     }
    //     return isMirror(left->left, right->right) && isMirror(left->right, right->left);
    // }

    bool isSymmetric(TreeNode* root) {
        if (!root) {
            return true;
        }
        queue<TreeNode*> q;
        q.push(root->left);
        q.push(root->right);
        while(!q.empty()) {
            TreeNode* l = q.front(); q.pop();
            TreeNode* r = q.front(); q.pop();
            if (!l && !r) {
                continue;
            }
            if (!l || !r) {
                return false;
            }
            if (l->val != r->val) {
                return false;
            }
            q.push(l->left);
            q.push(r->right);
            q.push(l->right);
            q.push(r->left);
        }
        return true;
    }
};