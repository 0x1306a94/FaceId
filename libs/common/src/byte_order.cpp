//
//  byte_order.cpp
//  face_id
//
//  Created by king on 2024/2/5.
//

#include "byte_order.hpp"

#ifdef __APPLE__
#include <CoreFoundation/CFByteOrder.h>
#else

#endif

namespace face {
namespace common {
ByteOrder NativeOrder() {
    #if defined(__LITTLE_ENDIAN__) // Apple
        return ByteOrder::LITTLE;
    #elif defined(__BIG_ENDIAN__) // Apple
        return ByteOrder::BIG;
    #elif defined(__BYTE_ORDER__) // GCC
        #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
            return ByteOrder::BIG;
        #else
            return ByteOrder::LITTLE;
        #endif
    #endif

    union test {
        int a;
        char b;
    } c;

    c.a = 1;
    return (c.b & 1 ? ByteOrder::LITTLE : ByteOrder::BIG);

    
}

bool NativeIsBig(void) {
    #if defined(__BIG_ENDIAN__) // Apple
        return true;
    #elif defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) // GCC
        return true;
    #endif
    return NativeOrder() == ByteOrder::BIG;
}

#ifdef __APPLE__

uint16_t SwapInt16HostToLittle(uint16_t arg) {
    return CFSwapInt16HostToLittle(arg);
}

uint16_t SwapInt16LittleToHost(uint16_t arg) {
    return CFSwapInt16LittleToHost(arg);
}

uint32_t SwapInt32HostToLittle(uint32_t arg) {
    return CFSwapInt32HostToLittle(arg);
}

uint32_t SwapInt32LittleToHost(uint32_t arg) {
    return CFSwapInt32LittleToHost(arg);
}

uint64_t SwapInt64HostToLittle(uint64_t arg) {
    return CFSwapInt64HostToLittle(arg);
}

uint64_t SwapInt64LittleToHost(uint64_t arg) {
    return CFSwapInt64LittleToHost(arg);
}

float SwapFloatHostToLittle(float arg) {
    #if __LITTLE_ENDIAN__
        return arg;
    #else
        return float_big_to_little(arg);
    #endif
}

float SwapFloatLittleToHost(float arg) {
    #if __LITTLE_ENDIAN__
        return arg;
    #else
        return float_little_to_big(arg);
    #endif
}

float float_little_to_big(float arg) {
    union FSwap {
        float v;
        struct {
            uint32_t v;
        } sv;
    } result;
    result.v = arg;
    result.sv.v = CFSwapInt32(result.sv.v);
    return result.v;
    // union {
    //     float f;
    //     unsigned char b[4];
    // } src, dest;
    // src.f = arg;
    // dest.b[0] = src.b[3];
    // dest.b[1] = src.b[2];
    // dest.b[2] = src.b[1];
    // dest.b[3] = src.b[0];
    // return dest.f;
}

float float_big_to_little(float arg) {
    union FSwap {
        float v;
        struct {
            uint32_t v;
        } sv;
    } result;
    result.v = arg;
    result.sv.v = CFSwapInt32(result.sv.v);
    return result.v;
    
    // union {
    //     float f;
    //     unsigned char b[4];
    // } src, dest;
    // src.f = arg;
    // dest.b[3] = src.b[0];
    // dest.b[2] = src.b[1];
    // dest.b[1] = src.b[2];
    // dest.b[0] = src.b[3];
    // return dest.f;
}

#else
// GCC
uint16_t SwapInt16HostToLittle(uint16_t arg) {
    #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        return __builtin_bswap16(arg);
    #else
        return arg;
    #endif
}

uint16_t SwapInt16LittleToHost(uint16_t arg) {
    #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        return __builtin_bswap16(arg);
    #else
        return arg;
    #endif
}

uint32_t SwapInt32HostToLittle(uint32_t arg) {
    #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        return __builtin_bswap32(arg);
    #else
        return arg;
    #endif
}

uint32_t SwapInt32LittleToHost(uint32_t arg) {
    #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        return __builtin_bswap32(arg);
    #else
        return arg;
    #endif
}

uint64_t SwapInt64HostToLittle(uint64_t arg) {
    #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        return __builtin_bswap64(arg);
    #else
        return arg;
    #endif
}

uint64_t SwapInt64LittleToHost(uint64_t arg) {
    #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        return __builtin_bswap64(arg);
    #else
        return arg;
    #endif
}

float SwapFloatHostToLittle(float arg) {
    #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        return float_big_to_little(arg);
    #else
        return arg;
    #endif
}

float SwapFloatLittleToHost(float arg) {
    #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        return float_little_to_big(arg);
    #else
        return arg;
    #endif
}

float float_little_to_big(float arg) {
    union FSwap {
        float v;
        struct {
            uint32_t v;
        } sv;
    } result;
    result.v = arg;
    result.sv.v = __builtin_bswap32(result.sv.v);
    return result.v;
    // union {
    //     float f;
    //     unsigned char b[4];
    // } src, dest;
    // src.f = arg;
    // dest.b[0] = src.b[3];
    // dest.b[1] = src.b[2];
    // dest.b[2] = src.b[1];
    // dest.b[3] = src.b[0];
    // return dest.f;
}

float float_big_to_little(float arg) {
    union FSwap {
        float v;
        struct {
            uint32_t v;
        } sv;
    } result;
    result.v = arg;
    result.sv.v = __builtin_bswap32(result.sv.v);
    return result.v;

    // union {
    //     float f;
    //     unsigned char b[4];
    // } src, dest;
    // src.f = arg;
    // dest.b[3] = src.b[0];
    // dest.b[2] = src.b[1];
    // dest.b[1] = src.b[2];
    // dest.b[0] = src.b[3];
    // return dest.f;
}

#endif

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
