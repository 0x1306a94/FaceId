#include "util.hpp"

#include <random>

namespace face {
namespace common {
namespace utils {
// 采样字符集
static constexpr char CCH[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

// sz: 字符串的长度
// printable：是否可打印。如果用作key，可以使用不可打印的字符哟
std::string makeRandStr(int sz, bool printable) {
    std::string ret;
    ret.resize(sz);
    std::mt19937 rng(std::random_device{}());
    for (int i = 0; i < sz; ++i) {
        if (printable) {
            uint32_t x = rng() % (sizeof(CCH) - 1);
            ret[i] = CCH[x];
        } else {
            ret[i] = rng() % 0xFF;
        }
    }

    return ret;
}

ByteOrder nativeOrder() {
    union test {
        int a;
        char b;
    } c;

    c.a = 1;
    return (c.b & 1 ? ByteOrder::LITTLE : ByteOrder::BIG);
}

float float_little_to_big(float src) {
    float dst;
    std::memcpy(&dst, &src, sizeof(float));
    char *dst_ptr = reinterpret_cast<char *>(&dst);
    std::swap(dst_ptr[0], dst_ptr[3]);
    std::swap(dst_ptr[1], dst_ptr[2]);
    return dst;
}

float float_big_to_little(float src) {
    float dst;
    std::memcpy(&dst, &src, sizeof(float));
    char *dst_ptr = reinterpret_cast<char *>(&dst);
    std::swap(dst_ptr[0], dst_ptr[3]);
    std::swap(dst_ptr[1], dst_ptr[2]);
    return dst;
}

std::ostream &operator<<(std::ostream &os, const ByteOrder &order) {
    if (order == ByteOrder::BIG) {
        os << "big endian";
    } else {
        os << "little endian";
    }

    return os;
}
}  // namespace utils
}  // namespace common
}  // namespace face
