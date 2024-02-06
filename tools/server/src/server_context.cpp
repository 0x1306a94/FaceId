//
//  server_context.cpp
//  face_id_server
//
//  Created by king on 2024/2/2.
//

#include "config.hpp"

#include "../recognizer/src/FeatureEnginePool.hpp"
#include "../storage/src/Storage.hpp"

#include "server_context.hpp"

namespace face {
namespace server {
static Context *g_context = nullptr;
Context::Context(std::shared_ptr<MainConfig> config, std::shared_ptr<storage::Storage> storage, std::shared_ptr<recognizer::FeatureEnginePool> enginePool)
    : m_config(config)
    , m_storage(storage)
    , m_enginePool(enginePool) {
}

void Context::Init(MainConfig config) {
    auto configPtr = std::make_shared<MainConfig>(config);
    auto storagePtr = std::make_shared<storage::Storage>(config.storage);

    const auto &recognizerConf = configPtr->recognizer;
    auto pool_size = recognizerConf.engine_pool_szie;
    auto model_dir = recognizerConf.face_model_dir;

    auto enginePoolPtr = std::make_shared<face::recognizer::FeatureEnginePool>(pool_size, model_dir);
    g_context = new Context(configPtr, storagePtr, enginePoolPtr);
}

Context *Context::Current() {
    return g_context;
}

std::shared_ptr<MainConfig> Context::GetConfig() {
    return m_config;
}

std::shared_ptr<storage::Storage> Context::GetStorage() {
    return m_storage;
}

std::shared_ptr<recognizer::FeatureEnginePool> Context::GetEnginePool() {
    return m_enginePool;
}

};  // namespace server
};  // namespace face
