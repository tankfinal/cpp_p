// =================================================================
// 題目二: 實作一個簡易的延遲任務調度器 (附 main 函式)
// =================================================================
#include <iostream>
#include <queue>
#include <vector>
#include <functional>
#include <chrono>
#include <thread>

/*
[核心觀念]
- task scheduling (任務調度)
- delay task (延遲任務)
- task prioritization (任務優先權)
*/

using TimePoint = std::chrono::steady_clock::time_point;

struct Task {
    int priority;
    std::function<void()> func;
    TimePoint wake_time;

    bool operator<(const Task& other) const {
        return priority < other.priority;
    }
};

struct DelayedTaskCompare {
    bool operator()(const Task& a, const Task& b) const {
        return a.wake_time > b.wake_time;
    }
};

class TaskScheduler {
private:
    std::priority_queue<Task> ready_queue_;
    std::priority_queue<Task, std::vector<Task>, DelayedTaskCompare> delayed_queue_;

public:
    void add_task(int priority, std::function<void()> func) {
        ready_queue_.push({priority, func, {}});
    }

    void run_task_after(std::chrono::milliseconds delay, int priority, std::function<void()> func) {
        TimePoint wake_time = std::chrono::steady_clock::now() + delay;
        delayed_queue_.push({priority, func, wake_time});
    }

    void run() {
        std::cout << "[Scheduler Start]" << std::endl;
        while (!ready_queue_.empty() || !delayed_queue_.empty()) {
            auto now = std::chrono::steady_clock::now();

            while (!delayed_queue_.empty() && delayed_queue_.top().wake_time <= now) {
                Task task = delayed_queue_.top();
                delayed_queue_.pop();
                std::cout << "[Scheduler] A delayed task is now ready." << std::endl;
                ready_queue_.push(task);
            }

            if (!ready_queue_.empty()) {
                Task task = ready_queue_.top();
                ready_queue_.pop();
                task.func();
            } else if (!delayed_queue_.empty()) {
                std::this_thread::sleep_until(delayed_queue_.top().wake_time);
            }
        }
        std::cout << "[Scheduler End]" << std::endl;
    }
};

// --- main 函式用於測試 ---
int main() {
    TaskScheduler scheduler;

    std::cout << "--- Testing Task Scheduler ---" << std::endl;

    // 1. 新增即時任務
    scheduler.add_task(10, []{ std::cout << "  Task A (Prio 10) executed." << std::endl; });
    scheduler.add_task(5,  []{ std::cout << "  Task B (Prio 5) executed." << std::endl; });
    scheduler.add_task(20, []{ std::cout << "  Task C (Prio 20) executed." << std::endl; });

    // 2. 新增延遲任務
    scheduler.run_task_after(std::chrono::milliseconds(1000), 15,
        []{ std::cout << "  Delayed Task D (Prio 15) executed after 1s." << std::endl; });
    scheduler.run_task_after(std::chrono::milliseconds(500), 25,
        []{ std::cout << "  Delayed Task E (Prio 25) executed after 0.5s." << std::endl; });

    // 3. 執行調度器
    scheduler.run();

    std::cout << "--- Test Ended ---" << std::endl;
    return 0;
}

/*
如何編譯與執行:
g++ your_file_name.cpp -std=c++11 -o program -pthread
*/