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
#include <vector>

namespace face {
namespace storage {
class FaceRecordORM;
class FaceRecord {
  public:
    std::int64_t identifier;
    std::string appId;
    std::string userId;
    std::uint16_t index;
    std::uint32_t offset;
    std::int64_t createDate;
    std::int64_t updateDate;
    std::vector<float> feature;
    explicit FaceRecord(const std::string &appId, const std::string &userId, std::uint16_t index, std::uint32_t offset);
    explicit FaceRecord(const FaceRecordORM &source);
};
}  // namespace storage
};  // namespace face

#endif /* face_id_storage_face_record_hpp */
