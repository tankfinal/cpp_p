/*
* 題目：LeetCode 111. Minimum Depth of Binary Tree
 * 連結：https://leetcode.com/problems/minimum-depth-of-binary-tree/
 *
 * 【Embedded 對應】
 * - BFS 層序搜尋：第一個遇到的葉節點所在層即最小深度；避免遞迴處理單子樹特例。
 */

#include <queue>
using namespace std;

class Solution_111 {
public:
    int minDepth(TreeNode* root) {
        if (!root) return 0;
        queue<pair<TreeNode*, int>> q; q.push({root, 1});
        while (!q.empty()) {
            auto [node, d] = q.front(); q.pop();
            if (!node->left && !node->right) return d;
            if (node->left)  q.push({node->left,  d + 1});
            if (node->right) q.push({node->right, d + 1});
        }
        return 0;
    }
};
