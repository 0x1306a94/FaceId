//
//  face_record_orm.hpp
//  face_id_storage
//
//  Created by king on 2024/2/3.
//

#ifndef face_id_storage_face_record_orm_hpp
#define face_id_storage_face_record_orm_hpp

#ifdef __APPLE__
#include <WCDBCpp/WCDBCpp.h>
#else
#include <WCDB/WCDBCpp.h>
#endif

#include <cstdint>
#include <string>

namespace face {
namespace storage {
class FaceRecordORM {
  public:
    std::int64_t identifier;
    std::string appId;
    std::string userId;
    std::uint16_t fileIndex;
    std::uint32_t fileOffset;
    std::int64_t createDate;
    std::int64_t updateDate;

    FaceRecordORM();
    FaceRecordORM(const std::string &appId, const std::string &userId, std::uint16_t fileIndex, std::uint32_t fileOffset);
    WCDB_CPP_ORM_DECLARATION(FaceRecordORM);

    static std::string TableName();

  private:
};
}  // namespace storage
};  // namespace face

#endif /* face_id_storage_face_record_orm_hpp */
