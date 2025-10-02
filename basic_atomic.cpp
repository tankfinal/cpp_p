#include <iostream>
#include <atomic>
#include <string>

// =================================================================
// 範例：std::atomic<int> 的基礎操作語法
// 目的：在單執行緒環境下，清晰地展示各個核心 atomic 函式的行為。
// =================================================================

// 一個輔助函式，用來印出 atomic 變數的狀態
void print_state(const std::string& operation, const std::atomic<int>& var) {
    std::cout << "After " << operation << ":\t atomic_var = " << var.load() << std::endl;
}

int main() {
    std::cout << "--- Basic Operations of std::atomic<int> ---" << std::endl;

    // 建立一個 atomic<int> 物件，並初始化為 10
    std::atomic<int> atomic_var{10};
    std::cout << "Initial state:\t\t atomic_var = " << atomic_var.load() << std::endl;
    std::cout << "-------------------------------------------" << std::endl;

    // --- 1. load(): 安全地讀取值 ---
    int current_value = atomic_var.load();
    std::cout << "1. load():" << std::endl;
    std::cout << "   - Loaded value: " << current_value << std::endl;
    print_state("load()", atomic_var);
    std::cout << "-------------------------------------------" << std::endl;

    // --- 2. store(): 安全地寫入值 ---
    std::cout << "2. store(25):" << std::endl;
    atomic_var.store(25);
    print_state("store(25)", atomic_var);
    std::cout << "-------------------------------------------" << std::endl;
    
    // --- 3. exchange(): 原子地交換值，並回傳舊值 ---
    std::cout << "3. exchange(33):" << std::endl;
    int old_value = atomic_var.exchange(33);
    std::cout << "   - exchange() returned old value: " << old_value << std::endl;
    print_state("exchange(33)", atomic_var);
    std::cout << "-------------------------------------------" << std::endl;
    
    // --- 4. fetch_add(): 原子地增加值，並回傳增加前的值 ---
    std::cout << "4. fetch_add(7):" << std::endl;
    int value_before_add = atomic_var.fetch_add(7);
    std::cout << "   - fetch_add() returned value before add: " << value_before_add << std::endl;
    print_state("fetch_add(7)", atomic_var);
    std::cout << "-------------------------------------------" << std::endl;

    // --- 5. compare_exchange_strong() (CAS): 比較並交換 ---
    // 這是 atomic 最強大的操作，我們示範一次成功和一次失敗

    // 5a. 成功的情況
    std::cout << "5a. compare_exchange_strong() - SUCCESS case:" << std::endl;
    int expected_value1 = 40; // 我們期望目前的值是 40
    std::cout << "   - Current value is " << atomic_var.load() << ", expected is " << expected_value1 << std::endl;
    bool success1 = atomic_var.compare_exchange_strong(expected_value1, 50); // 如果等於 40，就設為 50
    
    std::cout << "   - Operation success: " << (success1 ? "true" : "false") << std::endl;
    std::cout << "   - 'expected_value1' variable is now: " << expected_value1 << " (unchanged on success)" << std::endl;
    print_state("CAS(success)", atomic_var);
    std::cout << std::endl;

    // 5b. 失敗的情況
    std::cout << "5b. compare_exchange_strong() - FAILURE case:" << std::endl;
    int expected_value2 = 99; // 我們故意給一個錯誤的期望值 99
    std::cout << "   - Current value is " << atomic_var.load() << ", expected is " << expected_value2 << std::endl;
    bool success2 = atomic_var.compare_exchange_strong(expected_value2, 100); // 如果等於 99，就設為 100
    
    std::cout << "   - Operation success: " << (success2 ? "true" : "false") << std::endl;
    std::cout << "   - 'expected_value2' variable is now: " << expected_value2 << " (IMPORTANT: updated to current value on failure!)" << std::endl;
    print_state("CAS(failure)", atomic_var);
    std::cout << "-------------------------------------------" << std::endl;

    return 0;
}