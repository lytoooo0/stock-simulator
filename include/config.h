#ifndef CONFIG_H
#define CONFIG_H

#include <string>

constexpr char stock_buffer_name[] = "/stock_buffer";
constexpr char sync_buffer_name[] = "/sync_buffer";
constexpr int64_t shm_capacity = 2000;
constexpr int64_t max_stock_num = 100;
constexpr int64_t info_buffer_size = 100;
#endif
