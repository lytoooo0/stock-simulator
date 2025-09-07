#ifndef GENERATOR_H
#define GENERATOR_H

#include <cstdint>
#include <string>
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
    void update_info(uint64_t offset);
    void write_to_shm(const Stock &s, const uint64_t off);

    int stock_fd = -1;
    int sync_fd = -1;
    Stock *stock_buffer;
    shared::memory::SyncData *sync_data;
    const uint64_t stock_num = max_stock_num;
    const uint64_t freq_min   = 1000; /* ms */
    const uint64_t freq_range = 2000; /* ms */
    const std::string stock_buf_name = stock_buffer_name;
    const std::string sync_buf_name  = sync_buffer_name;
    static constexpr uint64_t shm_size = sizeof(Stock) * shm_capacity;
    uint64_t loop_count = 0;  // For benchmark
};

} // namespace stock

#endif
