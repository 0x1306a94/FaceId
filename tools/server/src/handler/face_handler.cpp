//
//  face_handler.cpp
//  face_id_server
//
//  Created by king on 2024/2/6.
//

#include "face_handler.hpp"

#include "spdlog_common.hpp"

#include "result.hpp"

#include "../src/server_context.hpp"
#include "../storage/src/Storage.hpp"
#include "../storage/src/face_record.hpp"

#include "../recognizer/src/FeatureEngine.hpp"
#include "../recognizer/src/FeatureEnginePool.hpp"

#include "common_handler.hpp"

#include "face_req_params.hpp"

#include <spdlog/fmt/chrono.h>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>

#include <hv/HttpContext.h>

using std::chrono::duration_cast;
using std::chrono::milliseconds;

namespace face {
namespace server {
namespace handler {
int Face::Add(const HttpContextPtr &ctx) {
    auto json = ctx->request->GetJson();
    face::server::reqparmas::FaceAddParams params;
    try {
        ns::from_json(json, params);
    } catch (const std::exception &err) {
        SPDLOG_ERROR("{}", err.what());
        return SendFail(ctx, 400, err.what());
    }

    auto storage = server::Context::Current()->GetStorage();
    auto enginePool = server::Context::Current()->GetEnginePool();

    face::recognizer::FeatureEnginePool::LockGuard fr(enginePool);

    spdlog::stopwatch sw;
    auto result = fr->ExtractFeatureFromBase64(params.image);
    SPDLOG_TRACE("人脸特征提取 base64 appId: {} userId: {} elapsed: {}", params.appId, params.userId, duration_cast<milliseconds>(sw.elapsed()));
    if (result.is_err()) {
        auto errorCode = result.err_value();
        return SendFail(ctx, errorCode, "");
    }
    auto value = result.ok_value();
    auto added = false;
    auto addResult = storage->AddFaceRecord(params.appId, params.userId, params.userInfo, value.feature);
    if (addResult) {
        added = true;
    }
    return SendSuccess(ctx, nlohmann::json{{"feature", value.feature}, {"added", added}});
}

int Face::Info(const HttpContextPtr &ctx) {
    return 200;
}

int Face::List(const HttpContextPtr &ctx) {
    auto json = ctx->request->GetJson();
    face::server::reqparmas::FaceListParams params;
    try {
        ns::from_json(json, params);
    } catch (const std::exception &err) {
        SPDLOG_ERROR("{}", err.what());
        return SendFail(ctx, 400, err.what());
    }
    auto storage = server::Context::Current()->GetStorage();
    auto faces = storage->GetFaceRecords(params.appId, params.userId, params.count);
    return SendSuccess(ctx, faces);
}
};  // namespace handler
};  // namespace server
};  // namespace face
