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
    #include <iomanip>
#endif

std::atomic<bool> keep_running{true};
void sig_handler(int) {keep_running = false;}

namespace stock {

Generator::Generator() {
    int oflag = O_CREAT | O_RDWR | O_TRUNC;
    shared::memory::init<shared::memory::Info>(info_buffer,
                                               &info_fd,
                                               sizeof(shared::memory::Info),
                                               stock_buf_name.c_str(),
                                               true,
                                               oflag);
    shared::memory::init<Stock>(stock_buffer,
        &stock_fd,
        shm_size,
        stock_buf_name.c_str(),
        true,
        oflag);
}

Generator::~Generator() {
    shared::memory::clean<Stock>(stock_buffer, &stock_fd, shm_size, stock_buf_name.c_str(), true);
    shared::memory::clean<shared::memory::Info>(info_buffer, &info_fd, sizeof(shared::memory::Info), info_buf_name.c_str(), true);
}

void Generator::run() {
    int64_t cnt = 0;
#ifdef ENABLE_BENCHMARK
    // TODO:
#endif
    while (keep_running) {
        int64_t freq = freq_min + random_u64() % freq_range;
        std::this_thread::sleep_for(std::chrono::milliseconds(freq));
        Stock stock = Stock(max_stock_num);
#ifdef ENABLE_DEBUGGING
        std::cout << "(" << freq << "ms) " << stock << std::endl;
#endif
        write_to_shm(stock, cnt++);
        cnt %= max_stock_num;
    }
#ifdef ENABLE_BENCHMARK
    // TODO:
#endif
}

void Generator::update_info(int64_t offset) {
#ifdef ENABLE_BENCHMARK
    // TODO:
#endif
    info_buffer->offset[0] = offset;
#ifdef ENABLE_BENCHMARK
    // TODO:
#endif
}

void Generator::write_to_shm(const Stock &s, int off) {
    stock_buffer[off] = s;
}

} // namespace stock

int main()
{
    std::signal(SIGINT, sig_handler);
#ifdef ENABLE_DEBUGGING
    std::cout << "Debug mode enabled" << std::endl;
#endif
    stock::Generator stock_generator;
    stock_generator.run();

    return 0;
}
