//
//  user.cpp
//  face_id
//
//  Created by king on 2024/2/5.
//

#include "user.hpp"

#include "./orm/user_orm.hpp"

namespace face {
namespace storage {
User::User(const std::string &appId, const std::string &userId, const std::string &info)
    : appId(appId)
    , userId(userId)
    , userInfo(info)
    , createDate(0)
    , updateDate(0) {
}

User::User(const UserORM &source)
    : appId(source.appId)
    , userId(source.userId)
    , userInfo(source.userInfo)
    , createDate(source.createDate)
    , updateDate(source.updateDate) {
}
}  // namespace storage
};  // namespace face
