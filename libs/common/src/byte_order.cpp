//
//  byte_order.cpp
//  face_id
//
//  Created by king on 2024/2/5.
//

#include "byte_order.hpp"

namespace face {
namespace common {
ByteOrder NativeOrder() {
    union test {
        int a;
        char b;
    } c;

    c.a = 1;
    return (c.b & 1 ? ByteOrder::LITTLE : ByteOrder::BIG);
}

float float_little_to_big(float x) {
    union {
        float f;
        unsigned char b[4];
    } src, dest;
    src.f = x;
    dest.b[0] = src.b[3];
    dest.b[1] = src.b[2];
    dest.b[2] = src.b[1];
    dest.b[3] = src.b[0];
    return dest.f;
}

float float_big_to_little(float x) {
    union {
        float f;
        unsigned char b[4];
    } src, dest;
    src.f = x;
    dest.b[3] = src.b[0];
    dest.b[2] = src.b[1];
    dest.b[1] = src.b[2];
    dest.b[0] = src.b[3];
    return dest.f;
}

std::ostream &operator<<(std::ostream &os, const ByteOrder &order) {
    if (order == ByteOrder::BIG) {
        os << "big endian";
    } else {
        os << "little endian";
    }

    return os;
}
};  // namespace common
};  // namespace face
