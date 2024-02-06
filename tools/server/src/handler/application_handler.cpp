//
//  application_handler.cpp
//  face_id_server
//
//  Created by king on 2024/2/6.
//

#include "application_handler.hpp"

#include "spdlog_common.hpp"

#include "result.hpp"

#include "../src/server_context.hpp"
#include "../storage/src/Storage.hpp"
#include "../storage/src/application.hpp"

#include "common_handler.hpp"

#include <hv/HttpContext.h>

namespace face {
namespace server {
namespace handler {
int Application::Add(const HttpContextPtr &ctx) {
    auto json = ctx->request->GetJson();
    if (!json.contains("appId")) {
        return SendFail(ctx, 400, "缺少appId");
    }
    if (!json["appId"].is_string()) {
        return SendFail(ctx, 400, "appId类型错误");
    }

    if (!json.contains("name")) {
        return SendFail(ctx, 400, "缺少name");
    }
    if (!json["name"].is_string()) {
        return SendFail(ctx, 400, "name类型错误");
    }

    auto appId = json["appId"].get<std::string>();
    auto name = json["name"].get<std::string>();
    if (appId.length() > 32) {
        return SendFail(ctx, 400, "appId最长32");
    }

    auto storage = Context::Current()->GetStorage();
    auto existApp = storage->GetApplication(appId);
    if (existApp) {
        return SendFail(ctx, 400, "应用已存在");
    }
    auto result = storage->AddApplication(appId, name);
    if (result.is_err()) {
        auto msg = result.take_err_value();
        return SendFail(ctx, 400, msg);
    }
    auto appInfo = result.take_ok_value();
    return SendSuccess(ctx, nlohmann::json{
                                {"appId", appInfo.appId},
                                {"name", appInfo.name},
                                {"createDate", appInfo.createDate},
                                {"updateDate", appInfo.updateDate},
                            });
}

int Application::Info(const HttpContextPtr &ctx) {
    auto json = ctx->request->GetJson();
    if (!json.contains("appId")) {
        return SendFail(ctx, 400, "缺少appId");
    }
    if (!json["appId"].is_string()) {
        return SendFail(ctx, 400, "appId类型错误");
    }
    auto appId = json["appId"].get<std::string>();
    auto storage = Context::Current()->GetStorage();
    auto existApp = storage->GetApplication(appId);
    if (existApp) {
        auto &appInfo = existApp.value();
        return SendSuccess(ctx, nlohmann::json{
                                    {"appId", appInfo.appId},
                                    {"name", appInfo.name},
                                    {"createDate", appInfo.createDate},
                                    {"updateDate", appInfo.updateDate},
                                });
    }
    return SendSuccess(ctx, {});
}

int Application::List(const HttpContextPtr &ctx) {
    auto json = ctx->request->GetJson();
    std::int64_t count = 0;
    if (json.contains("count")) {
        if (!json["count"].is_number_integer()) {
            return SendFail(ctx, 400, "count类型错误");
        }
        count = json["count"].get<std::int64_t>();
    }

    auto storage = Context::Current()->GetStorage();
    auto apps = storage->GetApplications(count);
    nlohmann::json json_list(nlohmann::json::array());
    for (const auto &app : apps) {
        json_list.emplace_back(nlohmann::json{
            {"appId", app.appId},
            {"name", app.name},
            {"createDate", app.createDate},
            {"updateDate", app.updateDate},
        });
    }
    return SendSuccess(ctx, std::move(json_list));
}
};  // namespace handler
};  // namespace server
};  // namespace face
