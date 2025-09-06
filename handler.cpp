#include <csignal>
#include <chrono>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <atomic>

#include "config.h"
#include "utils.h"
#include "handler.h"


std::atomic<bool> keep_running{true};
void sig_handler(int) {keep_running = false;}

namespace stock {

Handler::Handler(const int64_t lat): latency(lat) {
    // TODO: Read and write Info async, there might be conpetition
    shared::memory::init<Stock>(stock_buffer, &stock_fd, shm_size, stock_buf_name.c_str(), false);
}

Handler::~Handler() {
    shared::memory::clean<Stock>(stock_buffer, &stock_fd, shm_size, stock_buf_name.c_str());
}

void Handler::run() {
    std::clog << "start reading" << std::endl;
    int cnt = 0;
    while(keep_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(latency));
        std::clog << stock_buffer[cnt++] << std::endl;
        cnt %= max_stock_num;
    }
    std::clog << "finish reading" << std::endl;
}

} // namespace stock

int main()
{
    std::signal(SIGINT, sig_handler);
    stock::Handler hander;
    hander.run();
    return 0;
}
