//
//  user.hpp
//  face_id
//
//  Created by king on 2024/2/5.
//

#ifndef face_id_storage_user_hpp
#define face_id_storage_user_hpp

#include <cstdint>
#include <string>

namespace face {
namespace storage {
class UserORM;
class User {
  public:
    std::string appId;
    std::string userId;
    std::string userInfo;
    std::int64_t createDate;
    std::int64_t updateDate;

    explicit User(const std::string &appId, const std::string &userId, const std::string &info = "");
    explicit User(const UserORM &source);

    static User From(const UserORM &source);
};
}  // namespace storage
};  // namespace face

#endif /* face_id_storage_user_hpp */
