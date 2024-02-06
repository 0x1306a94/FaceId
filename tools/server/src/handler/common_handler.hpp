//
//  common_handler.hpp
//  face_id_server
//
//  Created by king on 2024/2/6.
//

#ifndef face_id_servercommon_handler_hpp
#define face_id_servercommon_handler_hpp

#include <hv/json.hpp>
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
int SendSuccess(const HttpContextPtr &ctx);
int SendSuccess(const HttpContextPtr &ctx, nlohmann::json result);
int SendFail(const HttpContextPtr &ctx, size_t code, const std::string &msg);
};  // namespace handler
};  // namespace server
};  // namespace face

#endif /* face_id_servercommon_handler_hpp */
