#include <iostream>

#include "rigtorp/MPMCQueue.h"

using RigtorpQ = rigtorp::MPMCQueue<int, rigtorp::shm::ShmAllocator_t>;

constexpr int QSize = 10;

static void recvFromClient() {
    int i = 0;
    RigtorpQ q(QSize, rigtorp::shm::ShmAllocator_t("rigtorp", rigtorp::shm::Mode_e::CONNECT));
    while (true) {
        int v;
        q.pop(v);
        std::cout << "Received " << v << std::endl;
    }
}

int main() {
    recvFromClient();
}
