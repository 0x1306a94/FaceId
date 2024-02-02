//
//  config.hpp
//  face_id
//
//  Created by king on 2024/2/2.
//

#ifndef __face_id_recognizer_config_h
#define __face_id_recognizer_config_h

#include <cstdint>
#include <string>
namespace face {
namespace recognizer {
struct Config {
    std::int16_t engine_pool_szie = 4;
    std::string face_model_dir = "";
};
}  // namespace recognizer
};  // namespace face

#endif /* __face_id_recognizer_config_h */
