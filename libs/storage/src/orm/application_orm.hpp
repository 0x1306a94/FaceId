//
//  application_orm.hpp
//  face_id
//
//  Created by king on 2024/2/4.
//

#ifndef application_orm_hpp
#define application_orm_hpp

#ifdef __APPLE__
#include <WCDBCpp/WCDBCpp.h>
#else
#include <WCDB/WCDBCpp.h>
#endif

#include <cstdint>
#include <string>
#include <vector>

namespace face {
namespace storage {
class ApplicationORM {
  public:
    std::uint64_t identifier;
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

#endif /* application_orm_hpp */
