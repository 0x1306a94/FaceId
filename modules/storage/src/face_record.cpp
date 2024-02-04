//
//  face_record.cpp
//  face_id_storage
//
//  Created by king on 2024/2/3.
//

#include "face_record.hpp"
namespace face {
namespace storage {

WCDB_CPP_ORM_IMPLEMENTATION_BEGIN(FaceRecord)

WCDB_CPP_SYNTHESIZE_COLUMN(identifier, "id")
WCDB_CPP_SYNTHESIZE_COLUMN(userId, "user_id")
WCDB_CPP_SYNTHESIZE_COLUMN(userInfo, "user_info")
WCDB_CPP_SYNTHESIZE_COLUMN(fileIndex, "file_index")
WCDB_CPP_SYNTHESIZE_COLUMN(fileOffset, "file_offset")
WCDB_CPP_SYNTHESIZE_COLUMN(createDate, "create_at")
WCDB_CPP_SYNTHESIZE_COLUMN(updateDate, "update_at")

WCDB_CPP_PRIMARY_AUTO_INCREMENT(identifier)

WCDB_CPP_DEFAULT(createDate, WCDB::LiteralValue::currentTimestamp())
WCDB_CPP_DEFAULT(updateDate, WCDB::LiteralValue::currentTimestamp())

WCDB_CPP_NOT_NULL(userId)
WCDB_CPP_NOT_NULL(fileIndex)
WCDB_CPP_NOT_NULL(fileOffset)
WCDB_CPP_NOT_NULL(createDate)
WCDB_CPP_NOT_NULL(updateDate)

WCDB_CPP_INDEX("_userIdIndex", userId)

WCDB_CPP_ORM_IMPLEMENTATION_END

std::string FaceRecord::TableName() {
    return std::string("face_record");
}

FaceRecord::FaceRecord()
    : FaceRecord("", -1, -1) {
}

FaceRecord::FaceRecord(const std::string &userId, long fileIndex, long fileOffset)
    : identifier(-1)
    , userId(userId)
    , userInfo(std::string())
    , fileIndex(fileIndex)
    , fileOffset(fileOffset) {
}
}  // namespace storage
};  // namespace face
