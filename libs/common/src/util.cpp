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

}  // namespace utils
}  // namespace common
}  // namespace face
