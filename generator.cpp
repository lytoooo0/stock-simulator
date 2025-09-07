#include <atomic>
#include <cassert>
#include <csignal>
#include <string>
#include <chrono>
#include <fcntl.h>
#include <cstdint>
#include <unistd.h>
#include <thread>
#include <sys/mman.h>
#include <sys/stat.h>

#include "utils.h"
#include "generator.h"

#ifdef ENABLE_DEBUGGING
    #include <iostream>
    #include <ostream>
#endif

std::atomic<bool> keep_running{true};
void sig_handler(int) {keep_running = false;}

namespace stock {

Generator::Generator() {
    int oflag = O_CREAT | O_RDWR | O_TRUNC;
    int mflag = PROT_READ | PROT_WRITE;
    shared::memory::init<Stock>(stock_buffer,
        &stock_fd,
        shm_size,
        stock_buf_name.c_str(),
        true,
        oflag,
        mflag);

    shared::memory::init<shared::memory::SyncData>(sync_data,
        &sync_fd,
        sizeof(shared::memory::SyncData),
        sync_buf_name.c_str(),
        true,
        oflag,
        mflag);
}

Generator::~Generator() {
    shared::memory::clean<Stock>(stock_buffer, &stock_fd, shm_size, stock_buf_name.c_str(), true);
    shared::memory::clean<shared::memory::SyncData>(sync_data, &sync_fd, sizeof(shared::memory::SyncData), sync_buf_name.c_str(), true);
}

void Generator::run() {
    int64_t offset = 0;
#ifdef ENABLE_BENCHMARK
    // TODO:
#endif
    while (keep_running) {
        int64_t freq = freq_min + random_u64() % freq_range;
        std::this_thread::sleep_for(std::chrono::milliseconds(freq));
        Stock stock = Stock(max_stock_num);
        write_to_shm(stock, offset);

        // Atomic coordination with memory barriers
        std::atomic_thread_fence(std::memory_order_release);
        sync_data->write_index.store(offset, std::memory_order_release);

#ifdef ENABLE_DEBUGGING
        std::clog << "(" << freq << " ms) " << stock_buffer[offset] << std::endl;
#endif
        offset = (offset + 1) % max_stock_num;
        loop_count++;
    }
#ifdef ENABLE_BENCHMARK
    // TODO:
#endif
}

void Generator::update_info(int64_t offset) {
#ifdef ENABLE_BENCHMARK
    // TODO:
#endif

#ifdef ENABLE_BENCHMARK
    // TODO:
#endif
}

void Generator::write_to_shm(const Stock &s, const int64_t off) {
    stock_buffer[off] = s;
}

} // namespace stock

int main()
{
    std::signal(SIGINT, sig_handler);
#ifdef ENABLE_DEBUGGING
    std::clog << "Debug mode enabled" << std::endl;
#endif
    stock::Generator stock_generator;
    stock_generator.run();

    return 0;
}
