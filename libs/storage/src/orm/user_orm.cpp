//
//  user_orm.cpp
//  face_id_storage
//
//  Created by king on 2024/2/5.
//

#include "user_orm.hpp"

namespace face {
namespace storage {

WCDB_CPP_ORM_IMPLEMENTATION_BEGIN(UserORM)

WCDB_CPP_SYNTHESIZE_COLUMN(appId, "app_id")
WCDB_CPP_SYNTHESIZE_COLUMN(userId, "user_id")
WCDB_CPP_SYNTHESIZE_COLUMN(userInfo, "user_info")
WCDB_CPP_SYNTHESIZE_COLUMN(createDate, "create_at")
WCDB_CPP_SYNTHESIZE_COLUMN(updateDate, "update_at")

WCDB_CPP_PRIMARY(userId)

WCDB_CPP_NOT_NULL(appId)
WCDB_CPP_NOT_NULL(userId)
WCDB_CPP_NOT_NULL(createDate)
WCDB_CPP_NOT_NULL(updateDate)

WCDB_CPP_UNIQUE_INDEX("_userid_appid_unique_index", userId)
WCDB_CPP_UNIQUE_INDEX("_userid_appid_unique_index", appId)

WCDB_CPP_INDEX("_create_at_index", createDate)

WCDB_CPP_ORM_IMPLEMENTATION_END

std::string UserORM::TableName() {
    return std::string("user");
}

UserORM::UserORM()
    : UserORM("", "", "") {
}

UserORM::UserORM(const std::string &appId, const std::string &userId, const std::string &info)
    : appId(appId)
    , userId(userId)
    , userInfo(userInfo) {
}
};  // namespace storage
};  // namespace face
