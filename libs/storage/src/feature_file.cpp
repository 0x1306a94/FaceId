//
//  feature_file.cpp
//  face_id
//
//  Created by king on 2024/2/2.
//

#include "feature_file.hpp"

#include <arpa/inet.h>

namespace face {
namespace storage {

#pragma pack(push, 1)
struct FeatureChunk {
    float value[1024];
};

struct FeatureFileHeader {
    char type[4] = {'f', 'a', 'c', 'e'};
    char appid[32];
    std::uint16_t serialNum;
    std::uint32_t count;
    FeatureChunk features[0];
};
#pragma pack(pop)

class FeatureFile::Implement {
  public:
    int m_fd;
    std::uint16_t m_serialNum;
    std::string m_appId;
    std::string m_dir;

    explicit Implement(const std::string &appId, std::uint16_t serialNum, const std::string &dir)
        : m_fd(-1)
        , m_appId(appId)
        , m_serialNum(serialNum)
        , m_dir(dir) {
    }

    ~Implement() {
    }
};

FeatureFile::FeatureFile(const std::string &appId, std::uint16_t serialNum, const std::string &dir)
    : impl(std::make_unique<Implement>(appId, serialNum, dir)) {
}
FeatureFile::~FeatureFile() {
}
}  // namespace storage
};  // namespace face
