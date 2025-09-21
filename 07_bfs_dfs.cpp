#include <iostream>
#include <vector>
#include <queue>
#include <stack>

/*
──────────────────────────────────────────────────────────────────────────────
[面試前備忘錄｜Google Embedded 視角]
• Graph traversal 是演算法高頻考點：BFS (queue) / DFS (stack/recursion)。
• BFS：無權圖最短路徑、最小步數問題；O(V+E)。
• DFS：檢查連通性、cycle 檢測、拓撲排序；O(V+E)。
• Embedded 特殊角度：
  - 若沒有 STL：要能自己實作固定大小 queue/stack。
  - 遞迴 DFS 可能爆棧，在資源有限的系統應用 iterative stack。
  - 資料結構應避免動態配置 → adjacency list 可用固定大小 array 模擬。
──────────────────────────────────────────────────────────────────────────────
[常見追問]
Q1: adjacency list vs adjacency matrix 差異？
A1: list → O(V+E)，適合稀疏圖；matrix → O(V^2)，適合稠密圖。

Q2: BFS vs DFS 用途差？
A2: BFS → 找最短路徑/最小步數；DFS → cycle 檢測、component 分析、拓撲排序。

Q3: 為什麼用 queue 實作 BFS？
A3: queue 保證 FIFO → 層次展開，第一次訪問就是最短距離。

Q4: 為什麼用 stack 模擬 DFS？
A4: 遞迴 DFS 可能爆棧；iterative DFS stack 可控，資源受限環境更安全。

──────────────────────────────────────────────────────────────────────────────
[陷阱備忘]
• 別解參考空 queue/stack → 先檢查 empty()。
• DFS 遞迴過深可能 stack overflow；特別是在 embedded 小 stack 系統。
• adjacency list push_back 在 embedded 可能觸發 heap → 可用固定大小容器。
──────────────────────────────────────────────────────────────────────────────
*/

// 新增無向邊 u-v
void add(std::vector<std::vector<int>>& g, int u, int v) {
    g[u].push_back(v);
    g[v].push_back(u);
}

int main() {
    int n = 5;
    std::vector<std::vector<int>> g(n);
    // Java 對照：List<List<Integer>> g = new ArrayList<>(n);

    // 建立圖
    add(g, 0, 1);
    add(g, 0, 2);
    add(g, 1, 3);
    add(g, 2, 4);
    // Graph (undirected):
    // 0 -- 1
    // |    |
    // 2    3
    // |
    // 4

    // ────────────────────────────────────────────────
    // BFS（廣度優先搜尋）
    // - 使用 queue
    // - dist[i] = 從起點到節點 i 的最短距離
    // - O(V+E)，適合找無權最短路徑
    // 【面試要點】
    //   • BFS = 層序遍歷，第一次訪問即最短路徑
    //   • 常見應用：最短步數、網路拓撲、迷宮走法
    //   • Embedded follow-up：若無 STL，要能用固定大小陣列實作 queue
    // ────────────────────────────────────────────────
    std::vector<int> dist(n, -1);
    std::queue<int> q;
    dist[0] = 0;   // 起點設為 0
    q.push(0);

    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (size_t i = 0; i < g[u].size(); ++i) {
            int v = g[u][i];
            if (dist[v] == -1) {  // 尚未訪問
                dist[v] = dist[u] + 1;
                q.push(v);
            }
        }
    }

    std::cout << "BFS dist: ";
    for (size_t i = 0; i < dist.size(); ++i) {
        std::cout << dist[i] << " ";
    }
    std::cout << "\n";

    // ────────────────────────────────────────────────
    // DFS（深度優先搜尋）— 用 stack 模擬遞迴
    // - O(V+E)，適合檢查連通性 / cycle / 拓撲排序
    // - 非遞迴版本 → 避免遞迴爆棧
    // 【面試要點】
    //   • DFS vs BFS：DFS 更適合 structural 分析；BFS 專注最短距離
    //   • iterative DFS 在 embedded 系統加分 → 不怕小 stack
    //   • follow-up：如何偵測 cycle？（若訪問到已在遞迴堆疊中的點）
    // ────────────────────────────────────────────────
    std::vector<int> vis(n, 0);
    std::stack<int> st;
    st.push(0);
    vis[0] = 1;

    std::cout << "DFS order: ";
    while (!st.empty()) {
        int u = st.top(); st.pop();
        std::cout << u << " ";
        for (size_t i = 0; i < g[u].size(); ++i) {
            int v = g[u][i];
            if (!vis[v]) {
                vis[v] = 1;
                st.push(v);
            }
        }
    }
    std::cout << "\n";

    /*
    ───────────────────────────────────────────────────────────
    [總結口條（可直接講）]
    • 這裡示範 BFS/DFS 的基本實作，皆為 O(V+E)。
    • BFS 適合找最短路徑；DFS 適合檢查結構（cycle、components）。
    • 在 embedded → 遞迴 DFS 可能爆棧，所以我用 stack iterative 版本。
    • adjacency list 在 embedded 可改為固定大小陣列，避免 heap。
    ───────────────────────────────────────────────────────────
    */

    return 0;
}
