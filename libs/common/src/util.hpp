#ifndef __util_hpp__
#define __util_hpp__

#include <cstring>
#include <iostream>
#include <string>
namespace face {
namespace common {
namespace utils {

enum class ByteOrder {
    BIG,
    LITTLE,
};

std::string makeRandStr(int sz, bool printable = true);

ByteOrder nativeOrder();

float float_little_to_big(float src);

float float_big_to_little(float src);

std::ostream &operator<<(std::ostream &os, const ByteOrder &order);
};  // namespace utils
};  // namespace common
}  // namespace face
#endif
