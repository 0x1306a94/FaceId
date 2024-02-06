//
//  application_handler.hpp
//  face_id_server
//
//  Created by king on 2024/2/6.
//

#ifndef application_handler_hpp
#define application_handler_hpp

#include <optional>

#ifndef HV_HTTP_CONTEXT_H_
namespace hv {
struct HttpContext;
};
typedef std::shared_ptr<hv::HttpContext> HttpContextPtr;
#endif

namespace face {
namespace server {
namespace handler {
struct Application {
    static int Add(const HttpContextPtr &ctx);
    static int Info(const HttpContextPtr &ctx);
    static int List(const HttpContextPtr &ctx);
};
};  // namespace handler
};  // namespace server
};  // namespace face

#endif /* application_handler_hpp */
