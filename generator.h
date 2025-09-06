#ifndef GENERATOR_H
#define GENERATOR_H

#include <cstdint>
#include <string>
#include <chrono>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "utils.h"
#include "config.h"

namespace stock {

class Generator {
public:
    Generator();
    ~Generator();

    void run();

private:
    void init_shm();
    void clean_shm();
    void update_info(int64_t offset);
    void write_to_shm(const Stock &s, int off);

    int stock_fd = -1;
    int info_fd = -1;
    Stock *stock_buffer;
    shared::memory::Info *info_buffer;
    const int64_t stock_num = max_stock_num;
    const int64_t freq_min   = 100; /* ms */
    const int64_t freq_range = 200; /* ms */
    const std::string stock_buf_name = stock_buffer_name;
    const std::string info_buf_name  = info_buffer_name;
    static constexpr int64_t shm_size = sizeof(Stock) * shm_capacity;
};

} // namespace stock

#endif
