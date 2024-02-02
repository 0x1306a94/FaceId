//
//  Storage.hpp
//  face_id
//
//  Created by king on 2024/2/2.
//

#ifndef Storage_hpp
#define Storage_hpp

#include "noncopyable.hpp"

#include <memory>
#include <string>

namespace face {
namespace storage {
class AppFile;
class Storage {
  public:
    FACE_MAKE_NONCOPYABLE(Storage);
    explicit Storage(const std::string &appId, const std::string &dataDir);
    ~Storage();

  private:
    std::unique_ptr<AppFile> m_app_file;
};
}  // namespace storage
}  // namespace face
#endif /* Storage_hpp */
