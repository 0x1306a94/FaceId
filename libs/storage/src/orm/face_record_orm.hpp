//
//  face_record_orm.hpp
//  face_id_storage
//
//  Created by king on 2024/2/3.
//

#ifndef face_record_orm_hpp
#define face_record_orm_hpp

#ifdef __APPLE__
#include <WCDBCpp/WCDBCpp.h>
#else
#include <WCDB/WCDBCpp.h>
#endif

#include <string>
#include <vector>

namespace face {
namespace storage {
class FaceRecordORM {
  public:
    std::int64_t identifier;
    std::int64_t appId;
    std::string userId;
    std::string userInfo;
    std::int64_t fileIndex;
    std::int64_t fileOffset;
    std::int64_t createDate;
    std::int64_t updateDate;

    FaceRecordORM();
    FaceRecordORM(std::int64_t appId, const std::string &userId, std::int64_t fileIndex, std::int64_t fileOffset);
    WCDB_CPP_ORM_DECLARATION(FaceRecordORM);

    static std::string TableName();

  private:
};
}  // namespace storage
};  // namespace face

#endif /* face_record_orm_hpp */
