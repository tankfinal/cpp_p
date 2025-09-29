#include <iostream>
#include <queue>   // std::queue
#include <stack>   // std::stack

int main() {
    std::queue<int> q;   // FIFO
    std::stack<int> s;   // LIFO

    // 放入元素 1,2,3,4,5
    for (int i = 1; i <= 5; i++) {
        q.push(i);
        s.push(i);
    }

    // Queue: FIFO (First In First Out)
    std::cout << "Queue pop順序: ";
    while (!q.empty()) {
        std::cout << q.front() << " "; // 取出最前面的
        q.pop();                       // 移除最前面的
    }
    std::cout << std::endl;

    // Stack: LIFO (Last In First Out)
    std::cout << "Stack pop順序: ";
    while (!s.empty()) {
        std::cout << static_cast<int>(s.top()) << " ";   // 取出最上面的
        s.pop();                       // 移除最上面的
    }
    std::cout << std::endl;

    return 0;
}
