#include <fcntl.h>
#include <random>
#include <iostream>
#include <ostream>
#include <iomanip>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "utils.h"

uint64_t random_u64(const uint64_t low, const uint64_t high)
{
    thread_local std::mt19937_64 eng(std::random_device{}());
    std::uniform_int_distribution<uint64_t> dist(low, high);
    return dist(eng);
}

double random_f64(const double low, const double high)
{
    thread_local std::mt19937_64 eng(std::random_device{}());
    std::uniform_real_distribution<double> dist(low, high);
    return dist(eng);
}

std::ostream &stock::operator<<(std::ostream &os, const Price &p) {
    os << std::fixed << std::setprecision(5) << "["
       << "o: " << p.open  << ", "
       << "h: " << p.high  << ", "
       << "l: " << p.low   << ", "
       << "c: " << p.close << "]";
    return os;
}
std::ostream &stock::operator<<(std::ostream &os, const Stock &s) {
    os << "id: " << std::setw(3) << s.get_id() << ", " << s.get_price();
    return os;
}

namespace stock {
Price::Price() {
    std::vector<double> vec = {random_f64(), random_f64(), random_f64(), random_f64()};
    std::sort(vec.begin(), vec.end());
    low   = vec[0];
    close = vec[1];
    open  = vec[2];
    high  = vec[3];
}

Stock::Stock(const uint64_t max_stock_num ) {
        timestamp = std::chrono::system_clock::now();
        price = Price();
        id = random_u64(0, max_stock_num);
    }

Stock::Stock(const uint64_t id_, const Price p_) : id(id_), price(p_) {};

uint64_t Stock::get_id() const {return id;}
Price Stock::get_price() const {return price;}
} // namespace stock
