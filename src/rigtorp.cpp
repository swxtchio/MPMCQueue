#include <vector>
#include <thread>
#include <chrono>
#include <iostream>

#include "rigtorp/MPMCQueue.h"

constexpr int QSize = 10;
constexpr int numThreads = 2;
constexpr int numEntriesPerThread = QSize / numThreads;

using RigtorpQ = rigtorp::MPMCQueue<int, rigtorp::shm::ShmAllocator_t>;

static void sendToClient() {
    int i = 0;
    RigtorpQ q(QSize, rigtorp::shm::ShmAllocator_t("rigtorp", rigtorp::shm::Mode_e::ALLOCATE));
    while (i < 10) {
        std::cout << "Pushing " << i << std::endl;
        q.push(i++);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

int main() {
    sendToClient();
}
