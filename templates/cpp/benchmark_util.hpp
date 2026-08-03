#include <chrono>
#include <functional>
#include <iostream>

inline void benchmark(std::function<void()> func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms = end - start;
    std::cout << "Execution time: " << ms.count() << " ms" << std::endl;
}
