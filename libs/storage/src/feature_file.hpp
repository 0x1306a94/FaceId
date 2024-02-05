//
//  feature_file.hpp
//  face_id
//
//  Created by king on 2024/2/2.
//

#ifndef face_id_storage_feature_file_hpp
#define face_id_storage_feature_file_hpp

#include "noncopyable.hpp"

#include <cstdint>
#include <memory>
#include <string>

namespace face {
namespace storage {
class FeatureFile {
  public:
    FACE_MAKE_NONCOPYABLE(FeatureFile);
    explicit FeatureFile(const std::string &appId, std::uint16_t serialNum, const std::string &dir);
    ~FeatureFile();

  private:
    class Implement;
    std::unique_ptr<Implement> impl;
};
}  // namespace storage
};  // namespace face

#endif /* face_id_storage_feature_file_hpp */
