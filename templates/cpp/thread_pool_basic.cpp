#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

std::mutex mtx;

void worker(int id) {
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << "Worker " << id << " executing thread " << std::this_thread::get_id() << std::endl;
}

int main() {
    std::vector<std::thread> threads;
    for(int i = 0; i < 5; ++i) threads.emplace_back(worker, i);
    for(auto& t : threads) t.join();
    return 0;
}
