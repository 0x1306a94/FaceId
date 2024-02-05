//
//  Storage.cpp
//  face_id
//
//  Created by king on 2024/2/2.
//

#include "Storage.hpp"
#include "config.hpp"
#include "feature_file.hpp"
#include "spdlog_common.hpp"

#include "./orm/application_orm.hpp"
#include "./orm/face_record_orm.hpp"
#include "./orm/user_orm.hpp"

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

#include <mutex>
#include <unordered_map>

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
    std::mutex m_mutex;
    std::unordered_map<std::string, std::vector<std::shared_ptr<FeatureFile>>> m_featureFiles;
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

        if (this->m_db->createTable<ApplicationORM>(ApplicationORM::TableName())) {
            SPDLOG_INFO("createTable: {}", ApplicationORM::TableName());
        } else {
            SPDLOG_ERROR("createTable fail: {}", ApplicationORM::TableName());
        }

        if (this->m_db->createTable<UserORM>(UserORM::TableName())) {
            SPDLOG_INFO("createTable: {}", UserORM::TableName());
        } else {
            SPDLOG_ERROR("createTable fail: {}", UserORM::TableName());
        }

        if (this->m_db->createTable<FaceRecordORM>(FaceRecordORM::TableName())) {
            SPDLOG_INFO("createTable: {}", FaceRecordORM::TableName());
        } else {
            SPDLOG_ERROR("createTable fail: {}", FaceRecordORM::TableName());
        }
    }

    std::shared_ptr<FeatureFile> GetFeatureFile(const std::string &appid) {
        if (appid.empty()) {
            return nullptr;
        }
        std::lock_guard<std::mutex> lock(m_mutex);
        auto iter = m_featureFiles.find(appid);
        if (iter == m_featureFiles.end()) {
            auto file = std::make_shared<FeatureFile>(appid, 0, m_featureDir.string());
            std::vector<std::shared_ptr<FeatureFile>> files;
            files.push_back(file);
            m_featureFiles.emplace(appid, std::move(files));
            return file;
        }
        auto &files = iter->second;
        std::int16_t index = 0;
        for (auto file : files) {
            std::int16_t fileIndex = file->GetIndex();
            if (!file->IsFull()) {
                return file;
            }
            index = std::max(index, fileIndex);
        }

        auto file = std::make_shared<FeatureFile>(appid, index + 1, m_featureDir.string());
        files.push_back(file);
        return file;
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
        appOrm.createDate = common::date_util::CurrentMilliTimestamp();
        appOrm.updateDate = common::date_util::CurrentMilliTimestamp();

        auto fields = {
            WCDB_FIELD(ApplicationORM::appId),
            WCDB_FIELD(ApplicationORM::name),
            WCDB_FIELD(ApplicationORM::createDate),
            WCDB_FIELD(ApplicationORM::updateDate),
        };

        if (this->m_db->insertObjects<ApplicationORM>(appOrm, ApplicationORM::TableName(), fields)) {
            Application appInfo(appOrm);
            return face::common::Ok<Application>(appInfo);
        }

        return face::common::Err<std::string>("data write failure");
    }

    std::list<Application> Applocations(std::int64_t limit = 0) {
        auto select = this->m_db->prepareSelect<ApplicationORM>()
                          .onResultFields(ApplicationORM::allFields())
                          .fromTable(ApplicationORM::TableName())
                          .orders(WCDB_FIELD(ApplicationORM::createDate).asOrder(WCDB::Order::DESC));

        if (limit > 0) {
            select.limit(limit);
        }

        std::list<Application> results;
        auto objects = select.allObjects();
        if (objects.failed()) {
            return results;
        }
        for (const auto &obj : objects.value()) {
            results.emplace_back(obj);
        }
        return results;
    }

    bool AddFaceRecord(const std::string &appId, const std::string &userId, const std::string &userInfo, const std::vector<float> &feature) {
        auto featureFile = GetFeatureFile(appId);
        if (!featureFile) {
            return false;
        }

        return this->m_db->runTransaction([&](WCDB::Handle &handler) -> bool {
            auto existUser = handler.getFirstObject<UserORM>(UserORM::TableName(), {WCDB_FIELD(UserORM::appId) == appId && WCDB_FIELD(UserORM::userId) == userId});
            auto write = featureFile->AddFeature(feature);
            if (!write.has_value()) {
                return false;
            }

            auto ts = common::date_util::CurrentMilliTimestamp();
            UserORM user(appId, userId, userInfo);
            user.createDate = ts;
            user.updateDate = ts;
            if (existUser.failed()) {
                if (!handler.insertObjects<UserORM>(user, UserORM::TableName())) {
                    return false;
                }
            } else {
                auto fields = {
                    WCDB_FIELD(UserORM::userInfo),
                    WCDB_FIELD(UserORM::updateDate),
                };
                if (!handler.updateObject<UserORM>(user, fields, UserORM::TableName())) {
                    return false;
                }
            }
            auto fileIndex = featureFile->GetIndex();
            FaceRecordORM face(appId, userId, fileIndex, write.value());
            face.isAutoIncrement = true;
            face.createDate = ts;
            face.updateDate = ts;
            auto fields = {
                WCDB_FIELD(FaceRecordORM::appId),
                WCDB_FIELD(FaceRecordORM::userId),
                WCDB_FIELD(FaceRecordORM::fileIndex),
                WCDB_FIELD(FaceRecordORM::fileOffset),
                WCDB_FIELD(FaceRecordORM::createDate),
                WCDB_FIELD(FaceRecordORM::updateDate),
            };
            auto res = handler.insertObjects<FaceRecordORM>(face, FaceRecordORM::TableName(), fields);
            return res;
        });
    }
};

Storage::Storage(const Config &config)
    : m_impl(std::make_unique<Implement>(config)) {
}

Storage::~Storage() {
}

std::vector<float> Storage::QueryFeature(const std::string &userId) {
    return m_impl->QueryFeature(userId);
}

face::common::Result<Application, std::string> Storage::AddApplication(const std::string &appid, const std::string &name) {
    return m_impl->AddApplication(appid, name);
}

std::list<Application> Storage::Applocations(std::int64_t limit) {
    return m_impl->Applocations(limit);
}

bool Storage::AddFaceRecord(const std::string &appId, const std::string &userId, const std::string &userInfo, const std::vector<float> &feature) {
    return m_impl->AddFaceRecord(appId, userId, userInfo, feature);
}
}  // namespace storage
}  // namespace face
