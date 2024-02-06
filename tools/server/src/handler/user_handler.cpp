//
//  user_handler.cpp
//  face_id_server
//
//  Created by king on 2024/2/6.
//

#include "user_handler.hpp"

#include "spdlog_common.hpp"

#include "result.hpp"

#include "../src/server_context.hpp"
#include "../storage/src/Storage.hpp"
#include "../storage/src/user.hpp"

#include "common_handler.hpp"

#include <hv/HttpContext.h>

namespace face {
namespace server {
namespace handler {
int User::Add(const HttpContextPtr &ctx) {
    auto json = ctx->request->GetJson();

    if (!json.contains("appId")) {
        return SendFail(ctx, 400, "缺少appId");
    }
    if (!json["appId"].is_string()) {
        return SendFail(ctx, 400, "appId类型错误");
    }

    if (!json.contains("userId")) {
        return SendFail(ctx, 400, "缺少userId");
    }
    if (!json["userId"].is_string()) {
        return SendFail(ctx, 400, "userId类型错误");
    }

    std::string userInfo;
    if (json.contains("userInfo")) {
        if (!json["userInfo"].is_string()) {
            return SendFail(ctx, 400, "userInfo类型错误");
        }
        userInfo = json["userInfo"].get<std::string>();
    }

    auto appId = json["appId"].get<std::string>();
    auto userId = json["userId"].get<std::string>();
    if (appId.length() > 32) {
        return SendFail(ctx, 400, "userId最长32");
    }

    if (userId.length() > 32) {
        return SendFail(ctx, 400, "userId最长32");
    }

    auto storage = Context::Current()->GetStorage();
    auto existUser = storage->GetUser(appId, userId);
    if (existUser) {
        return SendFail(ctx, 400, "用户已存在");
    }
    auto result = storage->AddUser(appId, userId, userInfo);
    if (result.is_err()) {
        auto msg = result.take_err_value();
        return SendFail(ctx, 400, msg);
    }
    auto user = result.take_ok_value();
    return SendSuccess(ctx, user);
}

int User::Info(const HttpContextPtr &ctx) {
    auto json = ctx->request->GetJson();
    if (!json.contains("appId")) {
        return SendFail(ctx, 400, "缺少appId");
    }
    if (!json["appId"].is_string()) {
        return SendFail(ctx, 400, "appId类型错误");
    }

    if (!json.contains("userId")) {
        return SendFail(ctx, 400, "缺少userId");
    }
    if (!json["userId"].is_string()) {
        return SendFail(ctx, 400, "userId类型错误");
    }

    auto appId = json["appId"].get<std::string>();
    auto userId = json["userId"].get<std::string>();
    auto storage = Context::Current()->GetStorage();
    auto existUser = storage->GetUser(appId, userId);
    if (existUser) {
        auto appInfo = existUser.value();
        return SendSuccess(ctx, appInfo);
    }
    return SendSuccess(ctx, {});
}

int User::List(const HttpContextPtr &ctx) {
    auto json = ctx->request->GetJson();
    std::int64_t count = 0;
    if (json.contains("count")) {
        if (!json["count"].is_number_integer()) {
            return SendFail(ctx, 400, "count类型错误");
        }
        count = json["count"].get<std::int64_t>();
    }

    auto storage = Context::Current()->GetStorage();
    auto users = storage->GetUsers(count);
    return SendSuccess(ctx, users);
}
};  // namespace handler
};  // namespace server
};  // namespace face
