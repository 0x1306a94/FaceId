//
//  application_orm.hpp
//  face_id
//
//  Created by king on 2024/2/4.
//

#ifndef face_id_storage_application_orm_hpp
#define face_id_storage_application_orm_hpp

#ifdef __APPLE__
#include <WCDBCpp/WCDBCpp.h>
#else
#include <WCDB/WCDBCpp.h>
#endif

#include <cstdint>
#include <string>

namespace face {
namespace storage {
class ApplicationORM {
  public:
    std::string appId;
    std::string name;
    std::uint64_t createDate;
    std::uint64_t updateDate;
    ApplicationORM();
    explicit ApplicationORM(const std::string &appId, const std::string &name);

    WCDB_CPP_ORM_DECLARATION(ApplicationORM);

    static std::string TableName();
};
}  // namespace storage
};  // namespace face

#endif /* face_id_storage_application_orm_hpp */
