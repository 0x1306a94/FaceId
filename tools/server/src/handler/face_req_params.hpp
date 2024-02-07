//
//  face_req_params.hpp
//  face_id_server
//
//  Created by king on 2024/2/6.
//

#ifndef face_id_serverface_req_params_hpp
#define face_id_serverface_req_params_hpp

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace face {
namespace storage {
class FaceRecord;
};
};  // namespace face

namespace face {
namespace server {
namespace reqparmas {
struct FaceAddParams {
    std::string appId{""};
    std::string userId{""};
    std::string userInfo{""};
    std::string image{""};
    bool encode{false};
};

struct FaceAddResponse {
    std::shared_ptr<face::storage::FaceRecord> info;
    bool added;
    std::vector<float> array;
    std::string encode;
    explicit FaceAddResponse(std::shared_ptr<face::storage::FaceRecord> info = nullptr)
        : info(info)
        , added(false)
        , array(0)
        , encode(std::string()) {}
};

struct FaceInfoParams {
    std::string appId{""};
    std::string userId{""};
    std::uint64_t faceId = 0;
};

struct FaceListParams {
    std::string appId{""};
    std::string userId{""};
    std::uint64_t count = 0;
};

};  // namespace reqparmas
};  // namespace server
};  // namespace face

#endif /* face_id_serverface_req_params_hpp */
