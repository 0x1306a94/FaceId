//
//  user_handler.hpp
//  face_id_server
//
//  Created by king on 2024/2/6.
//

#ifndef face_id_serveruser_handler_hpp
#define face_id_serveruser_handler_hpp

#include <memory>

#ifndef HV_HTTP_CONTEXT_H_
namespace hv {
struct HttpContext;
};
typedef std::shared_ptr<hv::HttpContext> HttpContextPtr;
#endif

namespace face {
namespace server {
namespace handler {

};
};  // namespace server
};  // namespace face

#endif /* face_id_serveruser_handler_hpp */
