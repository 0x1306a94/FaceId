//
//  Storage.cpp
//  face_id
//
//  Created by king on 2024/2/2.
//

#include "Storage.hpp"
#include "app_file.hpp"
#include "spdlog_common.hpp"

#include <WCDBCpp/WCDBCpp.h>

namespace face {
namespace storage {

class Storage::Implement {
  public:
    std::string m_appId;
    std::string m_dataDir;
    std::shared_ptr<WCDB::Database> m_db;
    explicit Implement(const std::string &appId, const std::string &dataDir)
        : m_appId(appId)
        , m_dataDir(dataDir)
        , m_db(nullptr) {

        this->m_db = std::make_shared<WCDB::Database>(dataDir + "/" + appId + ".db");
#ifndef NDEBUG
        this->m_db->traceSQL([](long tag, const WCDB::UnsafeStringView &path, const WCDB::UnsafeStringView &sql, const void *handleIdentifier) {
            SPDLOG_INFO("sql: {}", sql.data());
        });
#endif
        this->m_db->canOpen();
    }
};

Storage::Storage(const std::string &appId, const std::string &dataDir)
    : m_impl(std::make_unique<Implement>(appId, dataDir)) {
}

Storage::~Storage() {
}
}  // namespace storage
}  // namespace face
