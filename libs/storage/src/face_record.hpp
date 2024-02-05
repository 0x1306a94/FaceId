//
//  face_record.hpp
//  face_id
//
//  Created by king on 2024/2/5.
//

#ifndef face_id_storage_face_record_hpp
#define face_id_storage_face_record_hpp

#include <cstdint>
#include <string>

namespace face {
namespace storage {
class FaceRecordORM;
class FaceRecord {
  public:
    std::int64_t identifier;
    std::string appId;
    std::string userId;
    std::int64_t fileIndex;
    std::int64_t fileOffset;
    std::int64_t createDate;
    std::int64_t updateDate;
    explicit FaceRecord(const std::string &appId, const std::string &userId, std::int64_t fileIndex, std::int64_t fileOffset);
    explicit FaceRecord(const FaceRecordORM &source);
};
}  // namespace storage
};  // namespace face

#endif /* face_id_storage_face_record_hpp */
