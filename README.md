🎤 面試口條速記：Java ↔ C++ 對照表
1. 記憶體管理

Java：GC（Garbage Collector）自動釋放物件，生命週期不可控。

C++：RAII（Resource Acquisition Is Initialization），物件離開 scope 立刻解構，資源自動釋放。
👉 口條：「Java 用 GC，C++ 用 RAII，我可以更精確控制生命週期，像 FileHandle 自動關檔就是 RAII。」

2. 物件語義

Java：類別物件一律在 heap 上，用 new 建立，用 reference 存取。

C++：可以 stack 配置（快、可預測），或 heap 配置（需手動或用 smart pointer）。
👉 口條：「在 Java 我習慣 new，但 C++ 可以放在 stack 上更快；如果要 heap，就用 unique_ptr 管理。」

3. 指標 / 參考

Java：只有 reference（不能算術，不能空安全控制）。

C++：有 pointer（T* 可算術、可 nullptr）、有 reference（T& 更像 Java reference）。
👉 口條：「Java reference ≈ C++ reference；C++ pointer 更低階，可做算術，也可能 null。」

4. const / final

Java：final 用來標記不可變。

C++：const 用來保證不可修改；還可以放在函式後面限制成員不可修改。
👉 口條：「final 類似 const，但 C++ const 更靈活，可以修飾參數、指標、方法。」

5. 容器

Java：ArrayList, HashMap, Queue。

C++：std::vector, std::map, std::queue，性能更可控（stack / heap / allocator 可調整）。
👉 口條：「Java Collection 自帶 GC；C++ STL 容器可以選擇放 stack 或 heap，性能/記憶體可控。」

6. 多執行緒

Java：synchronized、volatile，GC/Thread 底層靠 JVM。

C++：std::mutex、std::atomic，要自己選 memory_order。
👉 口條：「Java synchronized 包含鎖和記憶體屏障；C++ mutex 更明確、atomic 可調 memory_order，更貼近硬體。」

7. Exception / Error

Java：Exception 層級（Checked / Unchecked）。

C++：有 exception，但 embedded 常禁用，因為成本高；多用 error code。
👉 口條：「在 Java 我會用 Exception；在嵌入式 C++ 更常用 error code 或狀態旗標，避免 exception runtime cost。」

8. 套件 / 命名空間

Java：package。

C++：namespace（例如 std::）。
👉 口條：「Java 用 package；C++ 用 namespace，避免符號衝突。」

🎯 面試官追問範例（你可這樣回）

Q：你主要是 Java，怎麼理解 C++ RAII？
A：「在 Java 我會用 try-with-resources；在 C++ RAII 等於建構子拿資源，解構子自動釋放，像 FileHandle 離開 scope 自動 fclose。」

Q：Java 有 GC，那你怎麼避免 C++ memory leak？
A：「我會用 smart pointer（unique_ptr / shared_ptr）取代裸 new/delete；這跟 Java reference count 的語義比較像，但我有更清楚的所有權控制。」

Q：Java synchronized 跟 C++ mutex 有什麼不同？
A：「Java synchronized 是語法糖，包含鎖和記憶體屏障；C++ std::mutex 更顯式，搭配 lock_guard 可以確保解鎖安全，還能精確控制 memory_order。」