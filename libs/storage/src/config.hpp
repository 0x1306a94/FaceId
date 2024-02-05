//
//  config.hpp
//  face_id
//
//  Created by king on 2024/2/2.
//

#ifndef face_id_storage_config_h
#define face_id_storage_config_h

#include <cstdint>
#include <string>
namespace face {
namespace storage {
struct Config {
    std::string data_dir = "";
};
}  // namespace recognizer
};  // namespace face

#endif /* face_id_storage_config_h */
