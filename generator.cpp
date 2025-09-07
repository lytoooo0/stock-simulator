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

    sync_data->num_handlers.store(0, std::memory_order_relaxed);
    sync_data->stock_buf_idx.store(0, std::memory_order_relaxed);
}

Generator::~Generator() {
    shared::memory::clean<Stock>(stock_buffer, &stock_fd, shm_size, stock_buf_name.c_str(), true);
    shared::memory::clean<shared::memory::SyncData>(sync_data, &sync_fd, sizeof(shared::memory::SyncData), sync_buf_name.c_str(), true);
#ifdef ENABLE_DEBUGGING
    double total_time = 0.0;
    for (double time : duration_record) total_time += time;
    double avg_time = total_time / duration_record.size() / 1000;
    std::cout << "average time for generator to store stock info: " << avg_time << "us." << std::endl;
#endif
}

void Generator::run() {
    uint64_t offset = 0;
    while (keep_running) {
        uint64_t freq = freq_min + random_u64() % freq_range;
        std::this_thread::sleep_for(std::chrono::milliseconds(freq));
        Stock stock = Stock(max_stock_num);
        write_to_shm(stock, offset);
        update_info(offset);
        uint64_t current_handlers = sync_data->num_handlers.load(std::memory_order_relaxed);

#ifdef ENABLE_DEBUGGING
        std::clog << "(" << freq << " ms) "
                  << current_handlers << " handlers "
                  << stock_buffer[offset] << std::endl;
#endif
        offset = (offset + 1) % max_stock_num;
        loop_count++;
    }
}

// TODO: try inlining
void Generator::update_info(uint64_t offset) {
#ifdef ENABLE_BENCHMARK
    auto start = std::chrono::high_resolution_clock::now();
#endif
    std::atomic_thread_fence(std::memory_order_release);
    sync_data->stock_buf_idx.store(offset, std::memory_order_release);
#ifdef ENABLE_BENCHMARK
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    duration_record.push_back(duration);
#endif
}

void Generator::write_to_shm(const Stock &s, const uint64_t off) {
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
