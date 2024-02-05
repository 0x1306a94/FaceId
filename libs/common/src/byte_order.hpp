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

float float_little_to_big(float x);

float float_big_to_little(float x);

ByteOrder NativeOrder();

std::ostream &operator<<(std::ostream &os, const ByteOrder &order);

};  // namespace common
};  // namespace face

#endif /* face_id_common_byte_order_hpp */
