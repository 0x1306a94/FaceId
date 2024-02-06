//
//  face_req_params.hpp
//  face_id_server
//
//  Created by king on 2024/2/6.
//

#ifndef face_id_serverface_req_params_hpp
#define face_id_serverface_req_params_hpp

#include <string>
namespace face {
namespace server {
namespace reqparmas {
struct FaceAddParams {
    std::string appId{""};
    std::string userId{""};
    std::string userInfo{""};
    std::string image{""};
};

struct FaceListParams {
    std::string appId{""};
    std::string userId{""};
    std::int64_t count = 0;
};

};  // namespace reqparmas
};  // namespace server
};  // namespace face

#endif /* face_id_serverface_req_params_hpp */
