#ifndef HANDLER_H
#define HANDLER_H

#include "config.h"
#include "utils.h"

namespace stock {
class Handler {
public:
    Handler() = default;
    Handler(const int64_t lat);
    ~Handler();
    void run();
private:
    int id = 0;
    int stock_fd = -1;
    int info_fd = -1;
    Stock *stock_buffer;
    const int64_t latency = 100.0; /* ms */
    const std::string stock_buf_name = stock_buffer_name;
    const std::string info_buf_name  = info_buffer_name;
    static constexpr int64_t shm_size = sizeof(Stock) * shm_capacity;
};
} // namespace stock

#endif
