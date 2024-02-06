//
//  byte_order.hpp
//  face_id
//
//  Created by king on 2024/2/5.
//

#ifndef face_id_common_byte_order_hpp
#define face_id_common_byte_order_hpp

#include <iostream>

namespace face {
namespace common {
enum class ByteOrder {
    BIG,
    LITTLE,
};

ByteOrder NativeOrder();

inline bool NativeIsBig(void) {
    return NativeOrder() == ByteOrder::BIG;
}

uint16_t SwapInt16HostToLittle(uint16_t arg);
uint16_t SwapInt16LittleToHost(uint16_t arg);

uint32_t SwapInt32HostToLittle(uint32_t arg);
uint32_t SwapInt32LittleToHost(uint32_t arg);

uint64_t SwapInt64HostToLittle(uint64_t arg);
uint64_t SwapInt64LittleToHost(uint64_t arg);

float float_little_to_big(float x);
float float_big_to_little(float x);

std::ostream &operator<<(std::ostream &os, const ByteOrder &order);

};  // namespace common
};  // namespace face

#endif /* face_id_common_byte_order_hpp */
