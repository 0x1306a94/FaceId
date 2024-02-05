//
//  application.hpp
//  face_id
//
//  Created by king on 2024/2/4.
//

#ifndef face_id_storage_application_hpp
#define face_id_storage_application_hpp

#include <cstdint>
#include <string>
namespace face {
namespace storage {
class ApplicationORM;

class Application {
  public:
    std::string appId;
    std::string name;
    std::uint64_t createDate;
    std::uint64_t updateDate;
    explicit Application(const std::string &appId, const std::string &name);
    explicit Application(const ApplicationORM &source);
};
};  // namespace storage
};  // namespace face

#endif /* face_id_storage_application_hpp */
