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
    : config(std::make_shared<MainConfig>(config)) {
}

void Context::Init(MainConfig config) {
    g_context = new Context(config);
    g_context->m_storage = std::make_shared<storage::Storage>(config.storage);
}

Context *Context::Current() {
    return g_context;
}

std::shared_ptr<storage::Storage> Context::GetStorage() {
    return m_storage;
}

};  // namespace server
};  // namespace face
