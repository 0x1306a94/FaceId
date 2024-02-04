//
//  application_orm.cpp
//  face_id
//
//  Created by king on 2024/2/4.
//

#include "application_orm.hpp"

namespace face {
namespace storage {

WCDB_CPP_ORM_IMPLEMENTATION_BEGIN(ApplicationORM)

WCDB_CPP_SYNTHESIZE_COLUMN(identifier, "id")
WCDB_CPP_SYNTHESIZE_COLUMN(appId, "app_id")
WCDB_CPP_SYNTHESIZE(name)
WCDB_CPP_SYNTHESIZE_COLUMN(createDate, "create_at")
WCDB_CPP_SYNTHESIZE_COLUMN(updateDate, "update_at")

WCDB_CPP_PRIMARY_AUTO_INCREMENT(identifier)

WCDB_CPP_NOT_NULL(appId)
WCDB_CPP_NOT_NULL(createDate)
WCDB_CPP_NOT_NULL(updateDate)

WCDB_CPP_UNIQUE(appId)

WCDB_CPP_INDEX("_appid_index", appId)

WCDB_CPP_INDEX("_create_at_index", createDate)

WCDB_CPP_ORM_IMPLEMENTATION_END

ApplicationORM::ApplicationORM()
    : ApplicationORM("", "") {
}

ApplicationORM::ApplicationORM(const std::string &appId, const std::string &name)
    : appId(appId)
    , name(name) {
}

std::string ApplicationORM::TableName() {
    return "application";
}

}  // namespace storage
};  // namespace face
