#ifndef CONFIG_H
#define CONFIG_H

#include <cstdint>

constexpr char stock_buffer_name[] = "/stock_buffer";
constexpr char sync_buffer_name[] = "/sync_buffer";
constexpr uint64_t shm_capacity = 2000;
constexpr uint64_t max_stock_num = 100;
constexpr uint64_t info_buffer_size = 100;
#endif
