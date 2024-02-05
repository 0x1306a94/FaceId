//
//  face_record.cpp
//  face_id
//
//  Created by king on 2024/2/5.
//

#include "face_record.hpp"

#include "./orm/face_record_orm.hpp"

namespace face {
namespace storage {
FaceRecord::FaceRecord(const std::string &appId, const std::string &userId, std::int64_t fileIndex, std::int64_t fileOffset)
    : identifier(-1)
    , appId(appId)
    , userId(userId)
    , fileIndex(fileIndex)
    , fileOffset(fileOffset)
    , createDate(0)
    , updateDate(0) {
}

FaceRecord::FaceRecord(const FaceRecordORM &source)
    : identifier(source.identifier)
    , appId(source.appId)
    , userId(source.userId)
    , fileIndex(source.fileIndex)
    , fileOffset(source.fileOffset)
    , createDate(source.createDate)
    , updateDate(source.updateDate) {
}
}  // namespace storage
};  // namespace face
