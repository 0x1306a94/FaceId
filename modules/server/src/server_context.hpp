//
//  server_context.hpp
//  face_id_server
//
//  Created by king on 2024/2/2.
//

#ifndef server_context_hpp
#define server_context_hpp

#include "noncopyable.hpp"
#include <memory>

namespace face {
namespace storage {
class Storage;
};

namespace server {
struct MainConfig;

class Context {
  public:
    FACE_MAKE_NONCOPYABLE(Context);
    MainConfig config;

    explicit Context(MainConfig config);
    static void Init(MainConfig config);
    static Context *Current();

  private:
    std::shared_ptr<storage::Storage> m_storage;
};
};     // namespace server
};     // namespace face
#endif /* server_context_hpp */
