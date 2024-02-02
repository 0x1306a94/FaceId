//
//  app_file.hpp
//  face_id
//
//  Created by king on 2024/2/2.
//

#ifndef app_file_hpp
#define app_file_hpp

#include "noncopyable.hpp"

#include <string>
namespace face {
namespace storage {
class AppFile {
  public:
    FACE_MAKE_NONCOPYABLE(AppFile);
    explicit AppFile(const std::string &appId, const std::string &dataDir);
    ~AppFile();
private:
    std::string m_appId;
    std::string m_dataDir;
};
}  // namespace storage
};     // namespace face
#endif /* app_file_hpp */
