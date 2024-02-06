//
//  common_handler.cpp
//  face_id_cli
//
//  Created by king on 2024/2/6.
//

#include "common_handler.hpp"

#include <hv/HttpContext.h>

namespace face {
namespace server {
namespace handler {

int SendSuccess(const HttpContextPtr &ctx) {
    nlohmann::json json;
    json["code"] = 0;
    json["result"] = true;
    return ctx->send(json.dump());
}

int SendSuccess(const HttpContextPtr &ctx, nlohmann::json result) {
    nlohmann::json json;
    json["code"] = 0;
    json["result"] = result;
    return ctx->send(json.dump());
}

int SendFail(const HttpContextPtr &ctx, long code, const std::string &msg) {
    nlohmann::json json;
    json["code"] = code;
    json["msg"] = msg;
    return ctx->send(json.dump());
}
};  // namespace handler
};  // namespace server
};  // namespace face
