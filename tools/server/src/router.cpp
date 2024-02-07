
#include "config.hpp"
#include "router.hpp"
#include "server_context.hpp"

#include "./handler/common_handler.hpp"

#include "./handler/application_handler.hpp"
#include "./handler/face_handler.hpp"
#include "./handler/user_handler.hpp"

#include <hv/HttpService.h>

namespace face {
namespace server {
void Router::Register(hv::HttpService &service) {
    service.preprocessor = http_handler([](const HttpContextPtr &ctx) -> int {
        ctx->response->content_type = APPLICATION_JSON;
        if (ctx->method() == http_method::HTTP_POST && ctx->request->content_length > 0 && !ctx->is(APPLICATION_JSON)) {
            return handler::SendFail(ctx, HTTP_STATUS_BAD_REQUEST, "request body must be in json format");
        }
        return HTTP_STATUS_UNFINISHED;
    });

    service.POST("/v1/app/add", handler::Application::Add);
    service.POST("/v1/app/info", handler::Application::Info);
    service.POST("/v1/app/list", handler::Application::List);

    service.POST("/v1/user/add", handler::User::Add);
    service.POST("/v1/user/info", handler::User::Info);
    service.POST("/v1/user/list", handler::User::List);

    service.POST("/v1/face/add", handler::Face::Add);
    service.POST("/v1/face/info", handler::Face::Info);
    service.POST("/v1/face/feature", handler::Face::Feature);
    service.POST("/v1/face/list", handler::Face::List);
    service.POST("/v1/face/match", handler::Face::Match);
    service.POST("/v1/face/search", handler::Face::Search);

    //    service.POST("/extract_file", Handler::extractFeatureFromFile);
    //    service.POST("/extract_base64", Handler::extractFeatureFromBase64);
    //    service.POST("/extract_url", Handler::extractFeatureFromURL);
    //    service.POST("/compare", Handler::compareFeature);
    //    service.POST("/query", Handler::queryFeature);
    //    service.POST("/batchQuery", Handler::batchQueryFeature);
    service.GET("/paths", [&service](HttpRequest *req, HttpResponse *resp) {
        return resp->Json(service.Paths());
    });
}
}  // namespace server
}  // namespace face
