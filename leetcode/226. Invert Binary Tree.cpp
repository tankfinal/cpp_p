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
    TreeNode* invertTree(TreeNode* root) {
        if (!root) {
            return root;
        }
        stack<TreeNode*> st;
        st.push(root);
        while(!st.empty()) {
            auto a = st.top();
            st.pop();
            std::swap(a->left, a->right);
            if (a->left) {
                st.push(a->left);
            }
            if (a->right) {
                st.push(a->right);
            }
        }
        return root;
    }
    // TreeNode* invertTree(TreeNode* root) {
    //     if (!root) {
    //         return root;
    //     }
    //     reverse(root);
    //     return root;
    // }
    // void reverse(TreeNode* x) {
    //     if (!x) {
    //         return;
    //     }
    //     TreeNode* tmp = x->left;
    //     x->left = x->right;
    //     x->right = tmp;

    //     reverse(x->left);
    //     reverse(x->right);


    // }
};