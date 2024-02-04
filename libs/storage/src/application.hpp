//
//  application.hpp
//  face_id
//
//  Created by king on 2024/2/4.
//

#ifndef application_hpp
#define application_hpp

#include <cstdint>
#include <string>
namespace face {
namespace storage {
class Application {
  public:
    std::uint64_t identifier;
    std::string appId;
    std::string name;
    std::uint64_t createDate;
    std::uint64_t updateDate;
    explicit Application(const std::string &appId, const std::string &name);
};
};  // namespace storage
};  // namespace face

#endif /* application_hpp */
