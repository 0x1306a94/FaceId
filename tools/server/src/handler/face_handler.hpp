//
//  face_handler.hpp
//  face_id_server
//
//  Created by king on 2024/2/6.
//

#ifndef face_id_serverface_handler_hpp
#define face_id_serverface_handler_hpp

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
struct Face {
    static int Add(const HttpContextPtr &ctx);
    static int Info(const HttpContextPtr &ctx);
    static int Feature(const HttpContextPtr &ctx);
    static int List(const HttpContextPtr &ctx);
    static int Match(const HttpContextPtr &ctx);
    static int Search(const HttpContextPtr &ctx);
};
};  // namespace handler
};  // namespace server
};  // namespace face

#endif /* face_id_serverface_handler_hpp */
