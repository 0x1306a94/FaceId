//
//  Storage.cpp
//  face_id
//
//  Created by king on 2024/2/2.
//

#include "Storage.hpp"
#include "app_file.hpp"
#include "config.hpp"
#include "spdlog_common.hpp"

#include "./orm/application_orm.hpp"
#include "./orm/face_record_orm.hpp"

#include "application.hpp"

#include "../common/src/date_util.hpp"

#include <spdlog/fmt/chrono.h>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>

#ifdef __APPLE__
#include <WCDBCpp/WCDBCpp.h>
#else
#include <WCDB/WCDBCpp.h>
#endif

#include <filesystem>

namespace fs = std::filesystem;

using std::chrono::duration_cast;
using std::chrono::milliseconds;

namespace face {
namespace storage {

class Storage::Implement {
  public:
    Config m_config;
    fs::path m_dataDir;
    fs::path m_dbDir;
    fs::path m_featureDir;
    std::shared_ptr<WCDB::Database> m_db;
    explicit Implement(const Config &config)
        : m_config(config)
        , m_dataDir(config.data_dir)
        , m_dbDir(config.data_dir + "/db")
        , m_featureDir(config.data_dir + "/feature")
        , m_db(nullptr) {

        if (!fs::exists(m_dbDir)) {
            fs::create_directories(m_dbDir);
        }
        if (!fs::exists(m_featureDir)) {
            fs::create_directories(m_featureDir);
        }
        auto dbPath(m_dbDir);
        dbPath.append("metadata.db");
        this->m_db = std::make_shared<WCDB::Database>(dbPath.string());
        auto create = WCDB::StatementCreateTable();
        SPDLOG_INFO("db path: {}", dbPath.string());
#ifndef NDEBUG
        this->m_db->traceSQL([](long tag, const WCDB::UnsafeStringView &path, const WCDB::UnsafeStringView &sql, const void *handleIdentifier) {
            SPDLOG_INFO("sql: {}", sql.data());
        });
#endif

        this->m_db->traceError([](const WCDB::Error &error) {
            if (!error.isOK()) {
                SPDLOG_ERROR("{}", error.getMessage().data());
            }
        });

        if (this->m_db->createTable<FaceRecordORM>(FaceRecordORM::TableName())) {
            SPDLOG_INFO("createTable: {}", FaceRecordORM::TableName());
        }

        if (this->m_db->createTable<ApplicationORM>(ApplicationORM::TableName())) {
            SPDLOG_INFO("createTable: {}", ApplicationORM::TableName());
        }
    }

    std::array<float, 1024> GenFeature(void) {
        std::array<float, 1024> result;
        for (int i = 0; i < 1024; i++) {
            float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 0.01;
            result[i] = r;
        }
        return result;
    }

    void GenerateTestData() {
        //        std::size_t total = 100000;
        //        std::size_t batchSize = 5000;
        //        std::size_t step = total / batchSize;
        //
        //        spdlog::stopwatch sw;
        //        SPDLOG_INFO("开始生成10w测试数");
        //        for (std::size_t idx = 0; idx < step; idx++) {
        //            std::list<FaceRecordORM> records;
        //            for (std::size_t j = 0; j < batchSize; j++) {
        //                //                std::array<float, 1024> featureArray = GenFeature();
        //                //                WCDB::Data feature((unsigned char *)featureArray.data(), sizeof(float) * featureArray.size());
        //                //                memcpy(feature.data(), featureArray.data(), feature.size());
        //                records.emplace_back(this->m_appId, std::to_string(idx * batchSize + j + 1), idx, j);
        //                auto &record = records.back();
        //                record.isAutoIncrement = true;
        //                record.createDate = common::date_util::CurrentMilliTimestamp();
        //                record.updateDate = common::date_util::CurrentMilliTimestamp();
        //            }
        //            this->m_db->runTransaction([&](WCDB::Handle &handler) -> bool {
        //                bool ret = true;
        //                for (auto &record : records) {
        //                    auto fields = {
        //                        WCDB_FIELD(FaceRecordORM::appId),
        //                        WCDB_FIELD(FaceRecordORM::userId),
        //                        WCDB_FIELD(FaceRecordORM::userInfo),
        //                        WCDB_FIELD(FaceRecordORM::fileIndex),
        //                        WCDB_FIELD(FaceRecordORM::fileOffset),
        //                        WCDB_FIELD(FaceRecordORM::createDate),
        //                        WCDB_FIELD(FaceRecordORM::updateDate),
        //                    };
        //                    ret &= handler.insertObjects<FaceRecordORM>(record, FaceRecordORM::TableName(), fields);
        //                    record.identifier = handler.getLastInsertedRowID();
        //                }
        //                return ret;
        //            });
        //        }
        //        SPDLOG_INFO("生成10w测试数据耗时: {}", duration_cast<milliseconds>(sw.elapsed()));
    }

    void QueryTestData() {
        spdlog::stopwatch sw;
        SPDLOG_INFO("开始查询10w测试数");
        auto result = this->m_db->getAllObjects<FaceRecordORM>(FaceRecordORM::TableName());
        if (result.hasValue()) {
            auto &records = result.value();
        }

        SPDLOG_INFO("查询10w测试数据耗时: {}", duration_cast<milliseconds>(sw.elapsed()));
    }

    std::vector<float> QueryFeature(const std::string &userId) {
        return {};
        //        spdlog::stopwatch sw;
        //        auto select = this->m_db->prepareSelect<FaceRecordORM>()
        //                          .onResultFields(WCDB_FIELD(FaceRecordORM::feature))
        //                          .fromTable(FaceRecordORM::TableName())
        //                          .where(WCDB_FIELD(FaceRecordORM::userId) == userId)
        //                          .limit(1);
        //        ;
        //        SPDLOG_INFO("db查询耗时: {}", duration_cast<milliseconds>(sw.elapsed()));
        //        auto record = select.firstObject();
        //        if (record.failed()) {
        //            return {};
        //        }
        //        auto &data = record.value().feature;
        //        std::vector<float> result(1024);
        //        memcpy(result.data(), data.buffer(), data.size());
        //        return result;
    }

    face::common::Result<Application, std::string> AddApplication(const std::string &appid, const std::string &name) {
        if (appid.empty()) {
            return face::common::Err<std::string>("appid null");
        }

        if (appid.length() > 32) {
            return face::common::Err<std::string>("appid cannot exceed 32 characters");
        }

        if (name.empty()) {
            SPDLOG_ERROR("name null");
            return face::common::Err<std::string>("name null");
        }

        if (appid.length() > 48) {
            return face::common::Err<std::string>("name cannot exceed 48 characters");
        }

        auto handler = this->m_db->getHandle();
        auto select = this->m_db->prepareSelect<ApplicationORM>()
                          .onResultFields(ApplicationORM::allFields())
                          .fromTable(ApplicationORM::TableName())
                          .where(WCDB_FIELD(ApplicationORM::appId) == appid)
                          .limit(1);
        auto record = select.firstObject();
        if (record.succeed()) {
            return face::common::Err<std::string>("the appid application already exists");
        }

        ApplicationORM appOrm(appid, name);
        appOrm.isAutoIncrement = true;
        appOrm.createDate = common::date_util::CurrentMilliTimestamp();
        appOrm.updateDate = common::date_util::CurrentMilliTimestamp();

        auto fields = {
            WCDB_FIELD(ApplicationORM::appId),
            WCDB_FIELD(ApplicationORM::name),
            WCDB_FIELD(ApplicationORM::createDate),
            WCDB_FIELD(ApplicationORM::updateDate),
        };
        
        if (this->m_db->insertObjects<ApplicationORM>(appOrm, ApplicationORM::TableName(), fields)) {
            appOrm.identifier = *appOrm.lastInsertedRowID;
            Application appInfo(appid, name);
            appInfo.identifier = *appOrm.lastInsertedRowID;
            appInfo.createDate = appOrm.createDate;
            appInfo.updateDate = appOrm.updateDate;
            return face::common::Ok<Application>(appInfo);
        }

        return face::common::Err<std::string>("data write failure");
    }
};

Storage::Storage(const Config &config)
    : m_impl(std::make_unique<Implement>(config)) {
}

Storage::~Storage() {
}

void Storage::GenerateTestData() {
    m_impl->GenerateTestData();
}

void Storage::QueryTestData() {
    m_impl->QueryTestData();
}

std::vector<float> Storage::QueryFeature(const std::string &userId) {
    return m_impl->QueryFeature(userId);
}

face::common::Result<Application, std::string> Storage::AddApplication(const std::string &appid, const std::string &name) {
    return m_impl->AddApplication(appid, name);
}
}  // namespace storage
}  // namespace face
