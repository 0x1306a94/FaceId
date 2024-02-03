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
WCDB_CPP_SYNTHESIZE(feature)

WCDB_CPP_PRIMARY_AUTO_INCREMENT(identifier)
WCDB_CPP_NOT_NULL(userId)
WCDB_CPP_NOT_NULL(feature)

WCDB_CPP_INDEX("_userIdIndex", userId)

WCDB_CPP_ORM_IMPLEMENTATION_END

std::string FaceRecord::TableName() {
    return std::string("face_record");
}

FaceRecord::FaceRecord()
    : FaceRecord("", WCDB::Data::null()) {
}

FaceRecord::FaceRecord(const std::string &userId, WCDB::Data feature)
    : identifier(-1)
    , userId(userId)
    , userInfo(std::string())
    , feature(std::move(feature)) {
}
}  // namespace storage
};  // namespace face
