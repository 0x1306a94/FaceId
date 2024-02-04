//
//  Storage.cpp
//  face_id
//
//  Created by king on 2024/2/2.
//

#include "Storage.hpp"
#include "app_file.hpp"
#include "face_record.hpp"
#include "spdlog_common.hpp"

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
    std::string m_appId;
    fs::path m_dataDir;
    fs::path m_dbDir;
    fs::path m_featureDir;
    std::shared_ptr<WCDB::Database> m_db;
    explicit Implement(const std::string &appId, const std::string &dataDir)
        : m_appId(appId)
        , m_dataDir(dataDir)
        , m_dbDir(dataDir + "/" + appId + "/db")
        , m_featureDir(dataDir + "/" + appId + "/feature")
        , m_db(nullptr) {

        if (!fs::exists(m_dbDir)) {
            fs::create_directories(m_dbDir);
        }
        if (!fs::exists(m_featureDir)) {
            fs::create_directories(m_featureDir);
        }
        auto dbPath(m_dbDir);
        dbPath.append("record.db");
        this->m_db = std::make_shared<WCDB::Database>(dbPath.string());
        auto create = WCDB::StatementCreateTable();
        SPDLOG_INFO("db path: {}", dbPath.string());
#ifndef NDEBUG
        this->m_db->traceSQL([](long tag, const WCDB::UnsafeStringView &path, const WCDB::UnsafeStringView &sql, const void *handleIdentifier) {
            SPDLOG_INFO("sql: {}", sql.data());
        });
#endif

        auto success = this->m_db->createTable<FaceRecord>(FaceRecord::TableName());
        SPDLOG_INFO("createTable: {} {}", FaceRecord::TableName(), success);
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
        std::size_t total = 100000;
        std::size_t batchSize = 5000;
        std::size_t step = total / batchSize;

        spdlog::stopwatch sw;
        SPDLOG_INFO("开始生成10w测试数");
        for (std::size_t idx = 0; idx < step; idx++) {
            std::list<FaceRecord> records;
            for (std::size_t j = 0; j < batchSize; j++) {
                //                std::array<float, 1024> featureArray = GenFeature();
                //                WCDB::Data feature((unsigned char *)featureArray.data(), sizeof(float) * featureArray.size());
                //                memcpy(feature.data(), featureArray.data(), feature.size());
                records.emplace_back(std::to_string(idx * batchSize + j + 1), idx, j);
                auto &record = records.back();
                record.isAutoIncrement = true;
                record.createDate = common::date_util::CurrentMilliTimestamp();
                record.updateDate = common::date_util::CurrentMilliTimestamp();
            }
            this->m_db->runTransaction([&](WCDB::Handle &handler) -> bool {
                bool ret = true;
                for (auto &record : records) {
                    auto fields = {
                        WCDB_FIELD(FaceRecord::userId),
                        WCDB_FIELD(FaceRecord::userInfo),
                        WCDB_FIELD(FaceRecord::fileIndex),
                        WCDB_FIELD(FaceRecord::fileOffset),
                        WCDB_FIELD(FaceRecord::createDate),
                        WCDB_FIELD(FaceRecord::updateDate),
                    };
                    ret &= handler.insertObjects<FaceRecord>(record, FaceRecord::TableName(), fields);
                    record.identifier = handler.getLastInsertedRowID();
                }
                return ret;
            });
        }
        SPDLOG_INFO("生成10w测试数据耗时: {}", duration_cast<milliseconds>(sw.elapsed()));
    }

    void QueryTestData() {
        spdlog::stopwatch sw;
        SPDLOG_INFO("开始查询10w测试数");
        auto result = this->m_db->getAllObjects<FaceRecord>(FaceRecord::TableName());
        if (result.hasValue()) {
            auto &records = result.value();
        }

        SPDLOG_INFO("查询10w测试数据耗时: {}", duration_cast<milliseconds>(sw.elapsed()));
    }

    std::vector<float> QueryFeature(const std::string &userId) {
        return {};
        //        spdlog::stopwatch sw;
        //        auto select = this->m_db->prepareSelect<FaceRecord>()
        //                          .onResultFields(WCDB_FIELD(FaceRecord::feature))
        //                          .fromTable(FaceRecord::TableName())
        //                          .where(WCDB_FIELD(FaceRecord::userId) == userId)
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
};

Storage::Storage(const std::string &appId, const std::string &dataDir)
    : m_impl(std::make_unique<Implement>(appId, dataDir)) {
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
}  // namespace storage
}  // namespace face
