//
//  Storage.hpp
//  face_id
//
//  Created by king on 2024/2/2.
//

#ifndef face_id_storage_Storage_hpp
#define face_id_storage_Storage_hpp

#include "noncopyable.hpp"

#include "result.hpp"

#include <cstdint>
#include <list>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace face {
namespace storage {
class Config;
class AppFile;
class Application;
class FaceRecord;
class User;

struct FaceFeaturChunkInfo {
    std::uint16_t index;
    std::uint32_t start;
    std::uint32_t count;
    std::shared_ptr<float> feature;
    explicit FaceFeaturChunkInfo(std::uint16_t index, std::uint32_t start, std::uint32_t count, std::shared_ptr<float> feature)
        : index(index)
        , start(start)
        , count(count)
        , feature(feature) {
    }
};

class Storage {
  public:
    FACE_MAKE_NONCOPYABLE(Storage);
    explicit Storage(const Config &config);
    ~Storage();

    face::common::Result<Application, std::string> AddApplication(const std::string &appid, const std::string &name);
    std::list<Application> GetApplications(std::int64_t limit = 10);
    std::optional<Application> GetApplication(const std::string &appid);

    face::common::Result<User, std::string> AddUser(const std::string &appid, const std::string &userId, const std::string &userInfo);
    std::list<User> GetUsers(std::int64_t limit = 10);
    std::optional<User> GetUser(const std::string &appid, const std::string &userId);
    std::optional<User> GetUser(const std::string &appid, std::uint16_t index, std::uint32_t offset);

    std::optional<FaceRecord> AddFaceRecord(const std::string &appId, const std::string &userId, const std::string &userInfo, const std::vector<float> &feature);
    std::list<FaceRecord> GetFaceRecords(const std::string &appId, const std::string &userId, std::int64_t limit = 10);
    std::optional<FaceRecord> GetFaceRecord(const std::string &appId, const std::string &userId, std::int64_t faceId);
    std::optional<std::vector<float>> GetFaceFeature(const FaceRecord &record);
    std::vector<FaceFeaturChunkInfo> GetFaceFeatures(const std::string &appId);

  private:
    class Implement;
    std::unique_ptr<Implement> m_impl;
};
}  // namespace storage
}  // namespace face
#endif /* face_id_storage_Storage_hpp */
