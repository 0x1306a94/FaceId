
#ifndef __router_hpp__
#define __router_hpp__

#include <string>

namespace hv {
class HttpService;
};

namespace face {
namespace server {

class Router {
  public:
    static void Register(hv::HttpService &service);
};
}  // namespace server
}  // namespace face
#endif
