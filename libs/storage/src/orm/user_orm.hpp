//
//  user_orm.hpp
//  face_id_storage
//
//  Created by king on 2024/2/5.
//

#ifndef face_id_storage_user_orm_hpp
#define face_id_storage_user_orm_hpp

#ifdef __APPLE__
#include <WCDBCpp/WCDBCpp.h>
#else
#include <WCDB/WCDBCpp.h>
#endif

#include <cstdint>
#include <string>

namespace face {
namespace storage {
class UserORM {
  public:
    std::string appId;
    std::string userId;
    std::string userInfo;
    std::int64_t createDate;
    std::int64_t updateDate;

    UserORM();
    explicit UserORM(const std::string &appId, const std::string &userId, const std::string &info = "");
    WCDB_CPP_ORM_DECLARATION(UserORM);

    static std::string TableName();
};
};  // namespace storage
};  // namespace face

#endif /* face_id_storage_user_orm_hpp */
