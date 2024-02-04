#include "../recognizer/src/FeatureEnginePool.hpp"

#include "config.hpp"
#include "handler.hpp"
#include "router.hpp"
#include "server_context.hpp"

#include <hv/HttpService.h>

namespace face {
namespace server {
void Router::Register(hv::HttpService &service) {
    const auto &recognizerConf = Context::Current()->config.recognizer;
    auto pool_size = recognizerConf.engine_pool_szie;
    auto model_dir = recognizerConf.face_model_dir;

    static auto pool = std::make_shared<recognizer::FeatureEnginePool>(pool_size, model_dir);
    Handler::registerFeatureEnginePool(pool);

    service.POST("/extract_file", Handler::extractFeatureFromFile);
    service.POST("/extract_base64", Handler::extractFeatureFromBase64);
    service.POST("/extract_url", Handler::extractFeatureFromURL);
    service.POST("/compare", Handler::compareFeature);
    service.POST("/query", Handler::queryFeature);
    service.POST("/batchQuery", Handler::batchQueryFeature);
    service.GET("/paths", [&service](HttpRequest *req, HttpResponse *resp) {
        return resp->Json(service.Paths());
    });
}
}  // namespace server
}  // namespace face
