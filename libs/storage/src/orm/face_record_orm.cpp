//
//  face_record_orm.cpp
//  face_id_storage
//
//  Created by king on 2024/2/3.
//

#include "face_record_orm.hpp"
namespace face {
namespace storage {

WCDB_CPP_ORM_IMPLEMENTATION_BEGIN(FaceRecordORM)

WCDB_CPP_SYNTHESIZE_COLUMN(identifier, "id")
WCDB_CPP_SYNTHESIZE_COLUMN(appId, "app_id")
WCDB_CPP_SYNTHESIZE_COLUMN(userId, "user_id")
WCDB_CPP_SYNTHESIZE_COLUMN(userInfo, "user_info")
WCDB_CPP_SYNTHESIZE_COLUMN(fileIndex, "file_index")
WCDB_CPP_SYNTHESIZE_COLUMN(fileOffset, "file_offset")
WCDB_CPP_SYNTHESIZE_COLUMN(createDate, "create_at")
WCDB_CPP_SYNTHESIZE_COLUMN(updateDate, "update_at")

WCDB_CPP_PRIMARY_AUTO_INCREMENT(identifier)

WCDB_CPP_NOT_NULL(appId)
WCDB_CPP_NOT_NULL(userId)
WCDB_CPP_NOT_NULL(fileIndex)
WCDB_CPP_NOT_NULL(fileOffset)
WCDB_CPP_NOT_NULL(createDate)
WCDB_CPP_NOT_NULL(updateDate)

WCDB_CPP_UNIQUE_INDEX("_appid_userid_index", appId)
WCDB_CPP_UNIQUE_INDEX("_appid_userid_index", userId)

WCDB_CPP_INDEX("_create_at_index", createDate)

WCDB_CPP_ORM_IMPLEMENTATION_END

std::string FaceRecordORM::TableName() {
    return std::string("face_record");
}

FaceRecordORM::FaceRecordORM()
    : FaceRecordORM(0, "", -1, -1) {
}

FaceRecordORM::FaceRecordORM(std::int64_t appId, const std::string &userId, std::int64_t fileIndex, std::int64_t fileOffset)
    : identifier(-1)
    , appId(appId)
    , userId(userId)
    , userInfo(std::string())
    , fileIndex(fileIndex)
    , fileOffset(fileOffset) {
}
}  // namespace storage
};  // namespace face
