#ifndef UTILS_H
#define UTILS_H

#include <atomic>
#include <cstring>
#include <chrono>
#include <cstdint>
#include <ostream>
#include <iostream>
#include <cerrno>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdexcept>

uint64_t random_u64(const uint64_t low = 0,   const uint64_t high = UINT64_MAX);
double   random_f64(const double   low = 0.0, const double   high = 1.0);

namespace stock {
struct Price {
    double open;
    double high;
    double low;
    double close;
    Price();
};

class Stock {
public:
    Stock(const uint64_t max_stock_num = 10);
    Stock(const uint64_t id_, const Price p_);
    uint64_t get_id() const;
    Price get_price() const;
    std::chrono::system_clock::time_point get_timestamp() const;
private:
    uint64_t id;
    Price price;
    std::chrono::system_clock::time_point timestamp;
};

std::ostream &operator<<(std::ostream &os, const Price &p);
std::ostream &operator<<(std::ostream &os, const Stock &s);

} // namespace stock

namespace shared {
namespace memory {

struct alignas(64) SyncData {
    std::atomic<uint64_t> num_handlers{0};
    char pad1[64 - sizeof(std::atomic<uint64_t>)];
    std::atomic<uint64_t> stock_buf_idx{0};
    char pad2[64 - sizeof(std::atomic<uint64_t>)];
};

template<typename T>
void init(T *&addr, int *fd_p, const uint64_t size, const char *name,
          const bool allocate = false,
          const int oflag = (O_RDWR),
          const int mflag = (PROT_READ | PROT_WRITE),
          const mode_t mode = DEFFILEMODE)
{
    int fd = shm_open(name, oflag, mode);
    if (fd == -1) {
        throw std::runtime_error(std::string("shm_open failed: ") + strerror(errno));
    }
    if (allocate) {
        int ftrunc_rst = ftruncate(fd, size);
        if (ftrunc_rst == -1) {
            close(fd);
            throw std::runtime_error(std::string("ftruncate failed: ") + strerror(errno));
        }
    }
    addr = (T *) mmap(nullptr, size, mflag, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        close(fd);
        throw std::runtime_error(std::string("mmap failed: ") + strerror(errno));
    }
    *fd_p = fd;
    if (allocate) memset(addr, 0, size);
}

template<typename T>
void clean(T *addr, int *fd_p, const uint64_t size, const char *name, const bool deallocate = false) {
    if (munmap(addr, size) == -1) {
        std::cerr << "munmap error: "<< strerror(errno) << std::endl;
    }
    if (close(*fd_p) == -1) {
        std::cerr << "close error: "<< strerror(errno) << std::endl;
    }
    if (deallocate) {
        if (shm_unlink(name) == -1) {
            std::cerr << "shm_unlink error: "<< strerror(errno) << std::endl;
        }
    }
#ifdef ENABLE_DEBUGGING
    std::clog << "Shared memory cleaned up" << std::endl;
#endif
}

} // namespace memory
} // namespace shared
#endif
