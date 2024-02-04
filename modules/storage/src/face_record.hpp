//
//  face_record.hpp
//  face_id_storage
//
//  Created by king on 2024/2/3.
//

#ifndef face_record_hpp
#define face_record_hpp

#ifdef __APPLE__
#include <WCDBCpp/WCDBCpp.h>
#else
#include <WCDB/WCDBCpp.h>
#endif

#include <vector>
namespace face {
namespace storage {
class FaceRecord {
  public:
    long long identifier;
    std::string userId;
    std::string userInfo;
    long fileIndex;
    long fileOffset;
    std::uint64_t createDate;
    std::uint64_t updateDate;

    FaceRecord();
    FaceRecord(const std::string &userId, long fileIndex, long fileOffset);
    WCDB_CPP_ORM_DECLARATION(FaceRecord);

    static std::string TableName();

  private:
};
}  // namespace storage
};  // namespace face

#endif /* face_record_hpp */
