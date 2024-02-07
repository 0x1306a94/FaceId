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
FaceRecord::FaceRecord(const std::string &appId, const std::string &userId, std::uint16_t index, std::uint32_t offset)
    : identifier(-1)
    , appId(appId)
    , userId(userId)
    , index(index)
    , offset(offset)
    , createDate(0)
    , updateDate(0) {
}

FaceRecord::FaceRecord(const FaceRecordORM &source)
    : identifier(source.identifier)
    , appId(source.appId)
    , userId(source.userId)
    , index(source.fileIndex)
    , offset(source.fileOffset)
    , createDate(source.createDate)
    , updateDate(source.updateDate) {
}
}  // namespace storage
};  // namespace face
