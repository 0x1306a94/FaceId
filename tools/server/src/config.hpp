#ifndef __face_id_server_config_h
#define __face_id_server_config_h

#include <iostream>
#include <string>

#include "../recognizer/src/config.hpp"
#include "../storage/src/config.hpp"

#include <yaml-cpp/node/node.h>

namespace face {
namespace server {
struct Config {
    std::int16_t http_thread_num = 4;
    std::int16_t http_port = 6666;
    std::string http_log = "";
};

struct MainConfig {
    std::string appId;
    server::Config server;
    recognizer::Config recognizer;
    storage::Config storage;
};

}  // namespace server
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
struct convert<face::server::Config> {
    static Node encode(const face::server::Config &rhs) {
        Node node;
        node["http_thread_num"] = rhs.http_thread_num;
        node["http_port"] = rhs.http_port;
        node["http_log"] = rhs.http_log;
        return node;
    }

    static bool decode(const Node &node, face::server::Config &rhs) {

        if (node["http_thread_num"]) {
            std::int16_t num = node["http_thread_num"].as<std::int16_t>();
            std::int16_t min = 1;
            rhs.http_thread_num = std::max(min, num);
        }

        if (node["http_port"]) {
            rhs.http_port = node["http_port"].as<std::int16_t>();
        }
        if (node["http_log"]) {
            rhs.http_log = node["http_log"].as<std::string>();
        }
        return true;
    }
};

template <>
struct convert<face::server::MainConfig> {
    static Node encode(const face::server::MainConfig &rhs) {
        Node node;
        node["appId"] = rhs.appId;
        node["server"] = rhs.server;
        node["recognizer"] = rhs.recognizer;
        node["storage"] = rhs.storage;
        return node;
    }

    static bool decode(const Node &node, face::server::MainConfig &rhs) {
        rhs.appId = node["appId"].as<std::string>();

        if (node["server"]) {
            rhs.server = node["server"].as<face::server::Config>();
        }
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
#endif
