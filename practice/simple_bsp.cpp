#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <mutex>

// =================================================================
// 概念模擬：一個極簡的 BSP 結構 (附詳細執行流程 cout)
// =================================================================

// 為了讓多執行緒的 cout 輸出不錯亂，我們用一個全域 mutex 來保護它
std::mutex cout_mutex;

class Test {
public:
    int some_vars;
    std::atomic<int> atomic_vars{0}; // 新增一個 atomic<int> 來對比

    Test() : some_vars(0) {} // 初始化

    void run(const std::string &test) {
        std::cout << test << std::endl;

        const int increments = 100000;

        std::thread new_thread([&]() {
            for (int i = 0; i < increments; ++i) {
                some_vars++;      // 不安全的 ++
                atomic_vars++;    // 安全的 ++
            }
        });

        std::thread new_thread2([&]() {
            for (int i = 0; i < increments; ++i) {
                some_vars++;      // 不安全的 ++
                atomic_vars++;    // 安全的 ++
            }
        });

        new_thread.join();
        new_thread2.join();

        std::cout << "Expected value: " << increments * 2 << std::endl;
        std::cout << "Final some_vars (int): " << some_vars << std::endl;
        std::cout << "Final atomic_vars (atomic): " << atomic_vars.load() << std::endl;
    }
};

// --- 1. 模擬硬體層 (Memory-Mapped Registers) ---
namespace Hardware {
    volatile bool UART_TX_READY = true;
    volatile char UART_TX_REGISTER = '\0';
    volatile bool GPIO_PIN_5_STATE = false;
}

// --- 2. 模擬設備驅動層 (Device Drivers) ---
// 驅動程式直接操作硬體暫存器

void uart_putc(char c) {
    while (!Hardware::UART_TX_READY); // 忙碌等待硬體就緒

    // 使用 lock_guard 保護 cout，確保日誌輸出的原子性
    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "    [Driver] uart_putc: Writing char '" << c << "' to UART_TX_REGISTER." << std::endl;
    }
    Hardware::UART_TX_REGISTER = c;
}

bool gpio_read_pin(int pin_number) {
    if (pin_number == 5) {
        // 使用 lock_guard 保護 cout
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "    [Driver] gpio_read_pin(" << pin_number <<
                    "): Reading state from GPIO_PIN_5_STATE hardware." << std::endl;
        }
        return Hardware::GPIO_PIN_5_STATE;
    }
    return false;
}

// --- 3. 模擬硬體抽象層 (HAL) ---
// HAL 將底層的驅動操作，封裝成更易用的 API

class ConsoleHAL {
public:
    void print(const std::string &s) {
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "  [HAL] ConsoleHAL::print called. Relaying to low-level driver..." << std::endl;
        }
        for (char c: s) {
            uart_putc(c); // HAL 內部呼叫底層驅動
        }
    }
};

class ButtonHAL {
private:
    int pin_number_;

public:
    ButtonHAL(int pin) : pin_number_(pin) {
    }

    bool is_pressed() {
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "  [HAL] ButtonHAL::is_pressed called. Calling low-level driver..." << std::endl;
        }
        return gpio_read_pin(pin_number_); // HAL 內部呼叫底層驅動
    }
};

// --- 4. 模擬作業系統/應用程式層 ---
// OS 主迴圈，它只和 HAL 互動

void os_main_loop(ConsoleHAL &console, ButtonHAL &button) {
    console.print("OS Main Loop started. Waiting for button press...\n");

    bool was_pressed = false;
    int counter = 0;
    while (counter++ <= 50) {
        // 限制迴圈次數以結束
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "[OS] Main loop tick. Calling HAL to check button state..." << std::endl;
        }

        bool is_pressed = button.is_pressed(); // OS 呼叫 HAL

        if (is_pressed && !was_pressed) {
            {
                std::lock_guard<std::mutex> lock(cout_mutex);
                std::cout << "[OS] HAL reports button is pressed! Performing action." << std::endl;
            }
            console.print("Button press detected!\n");
        }
        was_pressed = is_pressed;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    console.print("OS Main Loop finished.\n");
}

// --- 5. `main` 函式模擬 Bootloader 和啟動流程 ---
int main() {
    std::cout << "[Bootloader] Power on. Initializing hardware..." << std::endl;

    ConsoleHAL console;
    ButtonHAL button(5);


    std::thread external_event_simulator([&]() {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "\n<<<<< [Hardware Event] Physical button pressed. Pin 5 goes HIGH >>>>>\n" << std::endl;
        }
        Hardware::GPIO_PIN_5_STATE = true;

        std::this_thread::sleep_for(std::chrono::seconds(1));

        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "\n<<<<< [Hardware Event] Physical button released. Pin 5 goes LOW >>>>>\n" << std::endl;
        }
        Hardware::GPIO_PIN_5_STATE = false;
    });

    std::cout << "[Bootloader] Hardware ready. Jumping to OS Main Loop...\n" << std::endl;

    os_main_loop(console, button);

    external_event_simulator.join();

    std::cout << "\n[System] Shutdown." << std::endl;


    // Test test;
    // test.run("start");
    return 0;
}
