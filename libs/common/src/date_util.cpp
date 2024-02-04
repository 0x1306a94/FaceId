//
//  date_util.cpp
//  face_id
//
//  Created by king on 2024/2/4.
//

#include "date_util.hpp"

#include <chrono>

namespace face {
namespace common::date_util {
std::int64_t CurrentMilliTimestamp(void) {
    auto now = std::chrono::system_clock::now();
    auto millisec_timestamp = std::chrono::time_point_cast<std::chrono::milliseconds>(now).time_since_epoch().count();
    return static_cast<std::int64_t>(millisec_timestamp);
}
};  // namespace common::date_util
};  // namespace face
