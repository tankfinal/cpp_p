/**
* Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode() : val(0), left(nullptr), right(nullptr) {}
 *     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
 *     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left),
 * right(right) {}
 * };
 */
class Solution {
public:
    int maxDepth(TreeNode* root) {
        //     if (!root)
        //         return 0;
        //     int l = maxDepth(root->left);
        //     int r = maxDepth(root->right);
        //     return 1 + std::max(l, r);

        //     if (!root) return 0;

        if (!root)
            return 0; // 別忘了空樹要處理

        int maxDepth = 0;
        stack<pair<TreeNode*, int>> st;
        st.push({root, 1});

        while (!st.empty()) {
            auto [node, depth] = st.top();
            st.pop();

            maxDepth = std::max(maxDepth, depth);

            if (node->left) {
                st.push({node->left, depth + 1});
            }
            if (node->right) {
                st.push({node->right, depth + 1});
            }
        }
        return maxDepth;
    }
};