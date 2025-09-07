#include <csignal>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <atomic>

#include "utils.h"
#include "handler.h"


std::atomic<bool> keep_running{true};
void sig_handler(int) {keep_running = false;}

namespace stock {

Handler::Handler(const uint64_t lat): latency(lat) {
    shared::memory::init<Stock>(stock_buffer, &stock_fd, shm_size, stock_buf_name.c_str(), false);
    shared::memory::init<shared::memory::SyncData>(sync_data, &sync_fd, sizeof(shared::memory::SyncData), sync_buf_name.c_str(), false);
    sync_data->num_handlers.fetch_add(1, std::memory_order_release);
}

Handler::~Handler() {
    if (sync_data != nullptr && sync_data != MAP_FAILED) {
        sync_data->num_handlers.fetch_sub(1, std::memory_order_release);
    }
    shared::memory::clean<Stock>(stock_buffer, &stock_fd, shm_size, stock_buf_name.c_str());
    shared::memory::clean<shared::memory::SyncData>(sync_data, &sync_fd, sizeof(shared::memory::SyncData), sync_buf_name.c_str());
}

void Handler::run() {
    std::clog << "start reading" << std::endl;
    uint64_t last_read_index = 0;

    while(keep_running) {
        uint64_t current_write_index = sync_data->stock_buf_idx.load(std::memory_order_acquire);
        if (current_write_index != last_read_index) {
            std::atomic_thread_fence(std::memory_order_acquire);
            std::clog << stock_buffer[current_write_index] << std::endl;
            last_read_index = current_write_index;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(latency));
    }
    std::clog << "finish reading" << std::endl;
}

} // namespace stock

int main(int argc, char* argv[])
{
    uint64_t latency = 190; /* ms */
    if (argc == 2) latency = atol(argv[1]);
    std::signal(SIGINT, sig_handler);
    stock::Handler hander(latency);
    hander.run();
    return 0;
}
