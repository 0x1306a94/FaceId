#ifndef __handler_hpp__
#define __handler_hpp__

#include <functional>
#include <memory>
#include <string>

#include <hv/HttpContext.h>

namespace face {
namespace recognizer {
class FeatureEnginePool;
};
namespace server {

class Handler {
  public:
    static void registerFeatureEnginePool(std::shared_ptr<recognizer::FeatureEnginePool> engine_pool);
    static int extractFeatureFromFile(const HttpContextPtr &ctx);
    static int extractFeatureFromBase64(const HttpContextPtr &ctx);
    static int extractFeatureFromURL(const HttpContextPtr &ctx);
    static int compareFeature(const HttpContextPtr &ctx);
    static int queryFeature(const HttpContextPtr &ctx);
    static int batchQueryFeature(const HttpContextPtr &ctx);
    static int sendFail(const HttpContextPtr &ctx, int code, const std::string &msg);
};
}  // namespace server
};  // namespace face
#endif
