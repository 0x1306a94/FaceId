//
//  server_context.cpp
//  face_id_server
//
//  Created by king on 2024/2/2.
//

#include "config.hpp"

#include "../storage/src/Storage.hpp"

#include "server_context.hpp"

namespace face {
namespace server {
static Context *g_context = nullptr;
Context::Context(MainConfig config)
    : config(config) {
}

void Context::Init(MainConfig config) {
    g_context = new Context(config);
    g_context->m_storage = std::make_shared<storage::Storage>(config.appId, config.storage.data_dir);
}

Context *Context::Current() {
    return g_context;
}

};  // namespace server
};  // namespace face
