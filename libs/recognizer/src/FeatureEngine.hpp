
#ifndef __FeatureEngine_hpp__
#define __FeatureEngine_hpp__

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "result.hpp"

namespace face {
namespace common {
class AutoBuffer;
}

namespace recognizer {
class FeatureEngine {
  public:
    enum GENDER : int {
        MALE,    /// 男
        FEMALE,  /// 女
    };

    struct Result {
        std::vector<float> feature;
        // GENDER gender;
    };

    enum ErrorCode : int {
        ImageFail = -1,
        NoFace = -2,
        MultiFace = -3,
        UnableFeature = -4,
    };

  private:
    class Impl;
    std::shared_ptr<Impl> impl;

  public:
    FeatureEngine(const std::string &model_dir);
    ~FeatureEngine();

    face::common::Result<FeatureEngine::Result, FeatureEngine::ErrorCode> ExtractFeatureFromBase64(const std::string &base64_str) const;
    face::common::Result<FeatureEngine::Result, FeatureEngine::ErrorCode> ExtractFeatureFromData(const std::string &source) const;
    face::common::Result<FeatureEngine::Result, FeatureEngine::ErrorCode> ExtractFeatureFromData(const face::common::AutoBuffer &buffer) const;
    face::common::Result<FeatureEngine::Result, FeatureEngine::ErrorCode> ExtractFeature(const std::string &image_path) const;
};
}  // namespace recognizer
};  // namespace face
#endif
