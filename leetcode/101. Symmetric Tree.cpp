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

    bool isSymmetric(TreeNode *root) {
        if (!root) return true;
        queue<TreeNode *> q;
        q.push(root->left);
        q.push(root->right);
        while (!q.empty()) {
            TreeNode *a = q.front();
            q.pop();
            TreeNode *b = q.front();
            q.pop();
            if (!a && !b) continue;
            if (!a || !b) return false;
            if (a->val != b->val) return false;
            q.push(a->left);
            q.push(b->right); // 外對外
            q.push(a->right);
            q.push(b->left); // 內對內
        }
        return true;
    }

    bool isSymmetric(TreeNode *root) {
        if (!root) return true;
        stack<TreeNode *> st;
        st.push(root->left);
        st.push(root->right);
        while (!st.empty()) {
            TreeNode *b = st.top();
            st.pop();
            TreeNode *a = st.top();
            st.pop();
            if (!a && !b) continue;
            if (!a || !b) return false;
            if (a->val != b->val) return false;
            st.push(a->left);
            st.push(b->right);
            st.push(a->right);
            st.push(b->left);
        }
        return true;
    }
};
