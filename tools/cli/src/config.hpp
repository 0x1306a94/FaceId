//
//  face_id_cli_config.hpp
//  face_id
//
//  Created by king on 2024/2/4.
//

#ifndef face_id_cli_config
#define face_id_cli_config

#include <string>

#include "../recognizer/src/config.hpp"
#include "../storage/src/config.hpp"

#include <yaml-cpp/node/node.h>

namespace face {
namespace cli {

struct MainConfig {
    recognizer::Config recognizer;
    storage::Config storage;
};

}  // namespace cli
};  // namespace face

namespace YAML {

template <>
struct convert<face::recognizer::Config> {
    static Node encode(const face::recognizer::Config &rhs) {
        Node node;
        node["engine_pool_szie"] = rhs.engine_pool_szie;
        node["face_model_dir"] = rhs.face_model_dir;
        return node;
    }

    static bool decode(const Node &node, face::recognizer::Config &rhs) {
        if (node["engine_pool_szie"]) {
            std::int16_t size = node["engine_pool_szie"].as<std::int16_t>();
            std::int16_t min = 1;
            rhs.engine_pool_szie = std::max(min, size);
        }
        rhs.face_model_dir = node["face_model_dir"].as<std::string>();
        return true;
    }
};

template <>
struct convert<face::storage::Config> {
    static Node encode(const face::storage::Config &rhs) {
        Node node;
        node["data_dir"] = rhs.data_dir;
        return node;
    }

    static bool decode(const Node &node, face::storage::Config &rhs) {
        rhs.data_dir = node["data_dir"].as<std::string>();
        return true;
    }
};

template <>
struct convert<face::cli::MainConfig> {
    static Node encode(const face::cli::MainConfig &rhs) {
        Node node;
        node["recognizer"] = rhs.recognizer;
        node["storage"] = rhs.storage;
        return node;
    }

    static bool decode(const Node &node, face::cli::MainConfig &rhs) {
        if (node["recognizer"]) {
            rhs.recognizer = node["recognizer"].as<face::recognizer::Config>();
        }
        if (node["storage"]) {
            rhs.storage = node["storage"].as<face::storage::Config>();
        }
        return true;
    }
};
}  // namespace YAML

#endif /* face_id_cli_config */
