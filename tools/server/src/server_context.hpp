//
//  server_context.hpp
//  face_id_server
//
//  Created by king on 2024/2/2.
//

#ifndef face_id_server_server_context_hpp
#define face_id_server_server_context_hpp

#include "noncopyable.hpp"

#include <memory>

namespace face {

namespace recognizer {
class FeatureEngine;
class FeatureEnginePool;
};  // namespace recognizer

namespace storage {
class Storage;
};

namespace server {
struct MainConfig;

class Context {
  public:
    FACE_MAKE_NONCOPYABLE(Context);

    explicit Context(std::shared_ptr<MainConfig> config, std::shared_ptr<storage::Storage> storage, std::shared_ptr<recognizer::FeatureEnginePool> enginePool);
    static void Init(MainConfig config);
    static Context *Current();
    std::shared_ptr<MainConfig> GetConfig();
    std::shared_ptr<storage::Storage> GetStorage();
    std::shared_ptr<recognizer::FeatureEnginePool> GetEnginePool();

  private:
    std::shared_ptr<storage::Storage> m_storage;
    std::shared_ptr<MainConfig> m_config;
    std::shared_ptr<recognizer::FeatureEnginePool> m_enginePool;
};
};     // namespace server
};     // namespace face
#endif /* face_id_server_server_context_hpp */
