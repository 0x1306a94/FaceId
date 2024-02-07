//
//  Storage.cpp
//  face_id
//
//  Created by king on 2024/2/2.
//

#include "Storage.hpp"
#include "config.hpp"
#include "feature_file.hpp"
#include "feature_file_header.h"
#include "spdlog_common.hpp"

#include "./orm/application_orm.hpp"
#include "./orm/face_record_orm.hpp"
#include "./orm/user_orm.hpp"

#include "application.hpp"
#include "face_record.hpp"
#include "user.hpp"

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

        if (!fs::exists(this->m_dbDir)) {
            fs::create_directories(this->m_dbDir);
        }
        if (!fs::exists(this->m_featureDir)) {
            fs::create_directories(this->m_featureDir);
        }
        auto dbPath(this->m_dbDir);
        dbPath.append("metadata.db");
        this->m_db = std::make_shared<WCDB::Database>(dbPath.string());
        auto create = WCDB::StatementCreateTable();
        SPDLOG_INFO("db path: {}", dbPath.string());
#ifndef NDEBUG
        this->m_db->traceSQL([](long tag, const WCDB::UnsafeStringView &path, const WCDB::UnsafeStringView &sql, const void *handleIdentifier) {
            SPDLOG_TRACE("sql: {}", sql.data());
        });
#endif

        this->m_db->traceError([](const WCDB::Error &error) {
            if (!error.isOK() && error.level != WCDB::Error::Level::Ignore) {
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

        this->LoadFeatureFiles();
    }

    void LoadFeatureFiles() {
        auto iter = fs::directory_iterator(this->m_featureDir);
        for (const auto &entry : iter) {
            if (!fs::is_regular_file(entry.path())) {
                continue;
            }
            auto filePath = entry.path();
            auto filename = filePath.filename().string();
            if (!filename.empty() && filename.at(0) == '.') {
                continue;
            }
            size_t underscore_pos = filename.find('_');
            if (underscore_pos == std::string::npos) {
                SPDLOG_ERROR("file name format is incorrect: {}", filePath.string());
                return 1;
            }
            std::string appid_str = filename.substr(0, underscore_pos);
            std::string index_str = filename.substr(underscore_pos + 1);
            std::uint16_t index = 0;
            try {
                index = static_cast<std::uint16_t>(std::stoul(index_str));
            } catch (std::exception &e) {
                SPDLOG_ERROR("{}", e.what());
                continue;
            }

            auto featureFile = std::make_shared<FeatureFile>(appid_str, index, this->m_featureDir.string());
            if (!featureFile->IsValid()) {
                continue;
            }
            if (auto value = this->m_featureFiles.find(appid_str); value != this->m_featureFiles.end()) {
                value->second.push_back(featureFile);
            } else {
                std::vector<std::shared_ptr<FeatureFile>> files;
                files.push_back(featureFile);
                this->m_featureFiles.emplace(appid_str, std::move(files));
            }
        }
    }

    std::vector<std::shared_ptr<FeatureFile>> GetFeatureFiles(const std::string &appid) {
        if (appid.empty()) {
            return {};
        }
        std::lock_guard<std::mutex> lock(this->m_mutex);
        auto iter = this->m_featureFiles.find(appid);
        if (iter == this->m_featureFiles.end()) {
            return {};
        }
        return iter->second;
    }

    std::shared_ptr<FeatureFile> GetWritableFeatureFile(const std::string &appid) {
        if (appid.empty()) {
            return nullptr;
        }
        std::lock_guard<std::mutex> lock(this->m_mutex);
        auto iter = this->m_featureFiles.find(appid);
        if (iter == this->m_featureFiles.end()) {
            auto file = std::make_shared<FeatureFile>(appid, 0, this->m_featureDir.string());
            std::vector<std::shared_ptr<FeatureFile>> files;
            files.push_back(file);
            this->m_featureFiles.emplace(appid, std::move(files));
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

        auto file = std::make_shared<FeatureFile>(appid, index + 1, this->m_featureDir.string());
        files.push_back(file);
        return file;
    }

    std::shared_ptr<FeatureFile> GetFeatureFile(const std::string &appid, std::int64_t index) {
        if (appid.empty() || index < 0) {
            return nullptr;
        }
        std::lock_guard<std::mutex> lock(this->m_mutex);
        auto iter = this->m_featureFiles.find(appid);
        if (iter == this->m_featureFiles.end()) {
            return nullptr;
        }
        for (auto &file : iter->second) {
            std::int16_t fileIndex = file->GetIndex();
            if (fileIndex == index) {
                return file;
            }
        }

        return nullptr;
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

        if (name.length() > 48) {
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

    std::list<Application> GetApplications(std::int64_t limit) {
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

    std::optional<Application> GetApplication(const std::string &appid) {
        auto select = this->m_db->prepareSelect<ApplicationORM>()
                          .onResultFields(ApplicationORM::allFields())
                          .fromTable(ApplicationORM::TableName())
                          .where(WCDB_FIELD(ApplicationORM::appId) == appid)
                          .limit(1);

        auto result = select.firstObject();
        if (result.succeed()) {
            return std::make_optional<Application>(result.value());
        }
        return std::nullopt;
    }

    face::common::Result<User, std::string> AddUser(const std::string &appid, const std::string &userId, const std::string &userInfo) {
        if (appid.empty()) {
            return face::common::Err<std::string>("appid null");
        }

        if (appid.length() > 32) {
            return face::common::Err<std::string>("appid cannot exceed 32 characters");
        }

        if (userId.empty()) {
            SPDLOG_ERROR("name null");
            return face::common::Err<std::string>("userId null");
        }

        if (userId.length() > 32) {
            return face::common::Err<std::string>("userId cannot exceed 48 characters");
        }

        if (userInfo.length() > 256) {
            return face::common::Err<std::string>("userInfo cannot exceed 256 characters");
        }

        auto app = this->GetApplication(appid);
        if (!app) {
            return face::common::Err<std::string>("Create an application first");
        }

        auto select = this->m_db->prepareSelect<UserORM>()
                          .onResultFields(UserORM::allFields())
                          .fromTable(UserORM::TableName())
                          .where(WCDB_FIELD(UserORM::appId) == appid && WCDB_FIELD(UserORM::userId) == userId)
                          .limit(1);
        auto record = select.firstObject();
        if (record.succeed()) {
            return face::common::Err<std::string>("the userId already exists");
        }

        UserORM userOrm(appid, userId, userInfo);
        userOrm.createDate = common::date_util::CurrentMilliTimestamp();
        userOrm.updateDate = common::date_util::CurrentMilliTimestamp();

        auto fields = {
            WCDB_FIELD(UserORM::appId),
            WCDB_FIELD(UserORM::userId),
            WCDB_FIELD(UserORM::userInfo),
            WCDB_FIELD(UserORM::createDate),
            WCDB_FIELD(UserORM::updateDate),
        };

        if (this->m_db->insertObjects<UserORM>(userOrm, UserORM::TableName(), fields)) {
            User userInfo(userOrm);
            return face::common::Ok<User>(userInfo);
        }

        return face::common::Err<std::string>("data write failure");
    }

    std::list<User> GetUsers(std::int64_t limit) {
        auto select = this->m_db->prepareSelect<UserORM>()
                          .onResultFields(UserORM::allFields())
                          .fromTable(UserORM::TableName())
                          .orders(WCDB_FIELD(UserORM::createDate).asOrder(WCDB::Order::DESC));

        if (limit > 0) {
            select.limit(limit);
        }

        std::list<User> results;
        auto objects = select.allObjects();
        if (objects.failed()) {
            return results;
        }
        for (const auto &obj : objects.value()) {
            results.emplace_back(obj);
        }
        return results;
    }

    std::optional<User> GetUser(const std::string &appid, const std::string &userId) {
        auto select = this->m_db->prepareSelect<UserORM>()
                          .onResultFields(UserORM::allFields())
                          .fromTable(UserORM::TableName())
                          .where(WCDB_FIELD(UserORM::appId) == appid && WCDB_FIELD(UserORM::userId) == userId)
                          .limit(1);

        auto result = select.firstObject();
        if (result.succeed()) {
            return std::make_optional<User>(result.value());
        }
        return std::nullopt;
    }

    std::optional<User> GetUser(const std::string &appid, std::uint16_t index, std::uint32_t offset) {
        auto select = this->m_db->prepareSelect<FaceRecordORM>()
                          .onResultFields(FaceRecordORM::allFields())
                          .fromTable(FaceRecordORM::TableName())
                          .where(WCDB_FIELD(FaceRecordORM::appId) == appid && WCDB_FIELD(FaceRecordORM::fileIndex) == index && WCDB_FIELD(FaceRecordORM::fileOffset) == offset)
                          .limit(1);

        auto result = select.firstObject();
        if (result.succeed()) {
            return this->GetUser(appid, result.value().userId);
        }
        return std::nullopt;
    }

    std::optional<FaceRecord> AddFaceRecord(const std::string &appId, const std::string &userId, const std::string userInfo, const std::vector<float> &feature) {
        auto app = this->GetApplication(appId);
        if (!app) {
            SPDLOG_ERROR("corresponding appid application does not exist: {}", appId);
            return std::nullopt;
        }

        auto featureFile = this->GetWritableFeatureFile(appId);
        if (!featureFile) {
            return std::nullopt;
        }

        std::optional<FaceRecord> result = std::nullopt;
        auto res = this->m_db->runTransaction([&](WCDB::Handle &handler) -> bool {
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
            face.identifier = *face.lastInsertedRowID;
            result = std::make_optional<FaceRecord>(face);
            return res;
        });

        return result;
    }

    std::list<FaceRecord> GetFaceRecords(const std::string &appId, const std::string &userId, std::int64_t limit) {
        auto select = this->m_db->prepareSelect<FaceRecordORM>()
                          .onResultFields(FaceRecordORM::allFields())
                          .fromTable(FaceRecordORM::TableName());

        if (userId.empty()) {
            select.where(WCDB_FIELD(FaceRecordORM::appId) == appId);
        } else {
            select.where(WCDB_FIELD(FaceRecordORM::appId) == appId && WCDB_FIELD(FaceRecordORM::userId) == userId);
        }

        select.orders(WCDB_FIELD(FaceRecordORM::createDate).asOrder(WCDB::Order::DESC));
        if (limit > 0) {
            select.limit(limit);
        }

        std::list<FaceRecord> results;
        auto objects = select.allObjects();
        if (objects.failed()) {
            return results;
        }
        for (const auto &obj : objects.value()) {
            results.emplace_back(obj);
        }
        return results;
    }

    std::optional<FaceRecord> GetFaceRecord(const std::string &appId, const std::string &userId, std::int64_t faceId) {
        auto app = this->GetApplication(appId);
        if (!app) {
            SPDLOG_ERROR("corresponding appid application does not exist: {}", appId);
            return std::nullopt;
        }

        auto user = this->GetUser(appId, userId);
        if (!user) {
            return std::nullopt;
        }

        auto select = this->m_db->prepareSelect<FaceRecordORM>()
                          .onResultFields(FaceRecordORM::allFields())
                          .fromTable(FaceRecordORM::TableName());

        select.where(WCDB_FIELD(FaceRecordORM::identifier) == faceId && WCDB_FIELD(FaceRecordORM::appId) == appId && WCDB_FIELD(FaceRecordORM::userId) == userId);
        select.limit(1);

        auto result = select.firstObject();
        if (result.succeed()) {
            return std::make_optional<FaceRecord>(result.value());
        }
        return std::nullopt;
    }

    std::optional<std::vector<float>> GetFaceFeature(const FaceRecord &record) {
        auto featureFile = this->GetFeatureFile(record.appId, record.index);
        if (!featureFile) {
            return std::nullopt;
        }
        auto result = featureFile->GetFeature(record.offset);
        return result;
    }

    std::vector<FaceFeaturChunkInfo> GetFaceFeatures(const std::string &appId) {
        auto files = this->GetFeatureFiles(appId);
        std::vector<FaceFeaturChunkInfo> result;
        for (const auto &file : files) {
            auto index = file->GetIndex();
            auto count = file->GetCount();
            if (count == 0) {
                continue;
            }
            void *ptr = malloc(count * FACE_FEATURE_SIZE);
            if (ptr == nullptr) {
                continue;
            }
            std::shared_ptr<float> memPtr((float *)ptr, [](float *ptr) {
                free(ptr);
            });
            if (!memPtr) {
                continue;
            }
            file->CopyFeature(0, count, memPtr.get());
            result.emplace_back(index, 0, count, memPtr);
        }
        return result;
    }
};

Storage::Storage(const Config &config)
    : m_impl(std::make_unique<Implement>(config)) {
}

Storage::~Storage() {
}

face::common::Result<Application, std::string> Storage::AddApplication(const std::string &appid, const std::string &name) {
    return this->m_impl->AddApplication(appid, name);
}

std::list<Application> Storage::GetApplications(std::int64_t limit) {
    return this->m_impl->GetApplications(limit);
}

std::optional<Application> Storage::GetApplication(const std::string &appid) {
    return this->m_impl->GetApplication(appid);
}

face::common::Result<User, std::string> Storage::AddUser(const std::string &appid, const std::string &userId, const std::string &userInfo) {
    return this->m_impl->AddUser(appid, userId, userInfo);
}

std::list<User> Storage::GetUsers(std::int64_t limit) {
    return this->m_impl->GetUsers(limit);
}

std::optional<User> Storage::GetUser(const std::string &appid, const std::string &userId) {
    return this->m_impl->GetUser(appid, userId);
}

std::optional<User> Storage::GetUser(const std::string &appid, std::uint16_t index, std::uint32_t offset) {
    return this->m_impl->GetUser(appid, index, offset);
}

std::optional<FaceRecord> Storage::AddFaceRecord(const std::string &appId, const std::string &userId, const std::string &userInfo, const std::vector<float> &feature) {
    return this->m_impl->AddFaceRecord(appId, userId, userInfo, feature);
}

std::list<FaceRecord> Storage::GetFaceRecords(const std::string &appId, const std::string &userId, std::int64_t limit) {
    return this->m_impl->GetFaceRecords(appId, userId, limit);
}

std::optional<FaceRecord> Storage::GetFaceRecord(const std::string &appId, const std::string &userId, std::int64_t faceId) {
    return this->m_impl->GetFaceRecord(appId, userId, faceId);
}

std::optional<std::vector<float>> Storage::GetFaceFeature(const FaceRecord &record) {
    return this->m_impl->GetFaceFeature(record);
}

std::vector<FaceFeaturChunkInfo> Storage::GetFaceFeatures(const std::string &appId) {
    return this->m_impl->GetFaceFeatures(appId);
}

}  // namespace storage
}  // namespace face
