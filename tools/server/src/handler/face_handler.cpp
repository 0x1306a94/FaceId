//
//  face_handler.cpp
//  face_id_server
//
//  Created by king on 2024/2/6.
//

#include "face_handler.hpp"

#include "spdlog_common.hpp"

#include "result.hpp"

#include "../src/SimilarityCalculator.hpp"

#include "../common/src/base64.hpp"
#include "../common/src/byte_order.hpp"

#include "../src/server_context.hpp"
#include "../storage/src/Storage.hpp"
#include "../storage/src/application.hpp"
#include "../storage/src/face_record.hpp"
#include "../storage/src/feature_file_header.h"
#include "../storage/src/user.hpp"

#include "../recognizer/src/FeatureEngine.hpp"
#include "../recognizer/src/FeatureEnginePool.hpp"

#include "common_handler.hpp"

#include "face_req_params.hpp"

#include <spdlog/fmt/chrono.h>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>

#include <hv/HttpContext.h>

#include <algorithm>
#include <queue>

using std::chrono::duration_cast;
using std::chrono::milliseconds;

namespace face {
namespace server {
namespace handler {

struct SimilarityResult {
    std::uint16_t index;
    std::uint32_t offset;
    float similarity;
    SimilarityResult(std::uint16_t index, std::uint32_t offset, float similarity)
        : index(index)
        , offset(offset)
        , similarity(similarity) {}
    bool operator<(const SimilarityResult &other) const {
        return similarity < other.similarity;
    }
};

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

    std::vector<float> faceFeature;
    do {
        auto enginePool = server::Context::Current()->GetEnginePool();

        face::recognizer::FeatureEnginePool::LockGuard fr(enginePool);

        spdlog::stopwatch sw;
        auto result = fr->ExtractFeatureFromBase64(params.image);
        SPDLOG_INFO("人脸特征提取 base64 appId: {} userId: {} elapsed: {}", params.appId, params.userId, duration_cast<milliseconds>(sw.elapsed()));
        if (result.is_err()) {
            auto errorCode = result.err_value();
            return SendFail(ctx, errorCode, "");
        }
        faceFeature = result.ok_value().feature;
    } while (0);

    auto addResult = storage->AddFaceRecord(params.appId, params.userId, params.userInfo, faceFeature);
    face::server::reqparmas::FaceAddResponse response;
    if (addResult) {
        response.added = true;
        response.info = std::make_shared<face::storage::FaceRecord>(addResult.value());
        if (params.encode) {
            if (common::NativeIsBig()) {
                for (auto &f : faceFeature) {
                    f = common::float_big_to_little(f);
                }
            }
            std::string source(reinterpret_cast<const char *>(faceFeature.data()), (sizeof(float) * faceFeature.size()));
            std::string base64_str = common::base64::to_base64(source);
            response.encode = base64_str;
        } else {
            response.array = faceFeature;
        }
    }
    return SendSuccess(ctx, response);
}

int Face::Info(const HttpContextPtr &ctx) {
    auto json = ctx->request->GetJson();
    face::server::reqparmas::FaceInfoParams params;
    try {
        ns::from_json(json, params);
    } catch (const std::exception &err) {
        SPDLOG_ERROR("{}", err.what());
        return SendFail(ctx, 400, err.what());
    }
    auto storage = server::Context::Current()->GetStorage();
    auto face = storage->GetFaceRecord(params.appId, params.userId, params.faceId);
    if (face) {
        return SendSuccess(ctx, face.value());
    }
    return SendSuccess(ctx, {});
}

int Face::Feature(const HttpContextPtr &ctx) {
    auto json = ctx->request->GetJson();
    face::server::reqparmas::FaceInfoParams params;
    try {
        ns::from_json(json, params);
    } catch (const std::exception &err) {
        SPDLOG_ERROR("{}", err.what());
        return SendFail(ctx, 400, err.what());
    }
    auto storage = server::Context::Current()->GetStorage();
    auto face = storage->GetFaceRecord(params.appId, params.userId, params.faceId);
    if (!face) {
        return SendSuccess(ctx, {});
    }
    auto result = storage->GetFaceFeature(face.value());
    if (!result) {
        return SendSuccess(ctx, {});
    }
    return SendSuccess(ctx, nlohmann::json{result.value()});
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

int Face::Match(const HttpContextPtr &ctx) {
    auto json = ctx->request->GetJson();
    face::server::reqparmas::FaceMatchParams params;
    try {
        ns::from_json(json, params);
    } catch (const std::exception &err) {
        SPDLOG_ERROR("{}", err.what());
        return SendFail(ctx, 400, err.what());
    }

    if (params.source.empty()) {
        return SendFail(ctx, 400, "source empty");
    }

    if (params.target.empty()) {
        return SendFail(ctx, 400, "target empty");
    }

    std::vector<float> sourceFeature;
    std::vector<float> targetFeature;

    do {
        auto enginePool = server::Context::Current()->GetEnginePool();
        face::recognizer::FeatureEnginePool::LockGuard fr(enginePool);

        spdlog::stopwatch sw;
        auto source = fr->ExtractFeatureFromBase64(params.source);
        SPDLOG_INFO("source人脸特征提取 base64 elapsed: {}", duration_cast<milliseconds>(sw.elapsed()));

        if (source.is_err()) {
            auto errorCode = source.err_value();
            return SendFail(ctx, errorCode, "source fail");
        }
        sw.reset();
        auto target = fr->ExtractFeatureFromBase64(params.target);
        SPDLOG_INFO("source人脸特征提取 base64 elapsed: {}", duration_cast<milliseconds>(sw.elapsed()));
        if (target.is_err()) {
            auto errorCode = target.err_value();
            return SendFail(ctx, errorCode, "target fail");
        }

        sourceFeature = source.ok_value().feature;
        targetFeature = target.ok_value().feature;
    } while (0);

    float similarity = SimilarityCalculator<algorithm::SeetaDefault>::calculate(sourceFeature.data(), targetFeature.data(), sourceFeature.size());
    face::server::reqparmas::FaceMatchResponse response;
    response.score = std::fmax(0.f, std::fmin(100.f, similarity * 100.0));
    return SendSuccess(ctx, response);
}

int Face::Search(const HttpContextPtr &ctx) {
    auto json = ctx->request->GetJson();
    face::server::reqparmas::FaceSearchParams params;
    try {
        ns::from_json(json, params);
    } catch (const std::exception &err) {
        SPDLOG_ERROR("{}", err.what());
        return SendFail(ctx, 400, err.what());
    }

    if (params.appId.empty()) {
        return SendFail(ctx, 400, "appId empty");
    }

    if (params.source.empty()) {
        return SendFail(ctx, 400, "source empty");
    }

    auto storage = server::Context::Current()->GetStorage();
    auto existApp = storage->GetApplication(params.appId);
    if (!existApp) {
        return SendFail(ctx, 400, "appid does not exist");
    }

    std::vector<float> sourceFeature;
    do {
        auto enginePool = server::Context::Current()->GetEnginePool();
        face::recognizer::FeatureEnginePool::LockGuard fr(enginePool);

        spdlog::stopwatch sw;
        auto source = fr->ExtractFeatureFromBase64(params.source);
        SPDLOG_INFO("source人脸特征提取 base64 elapsed: {}", duration_cast<milliseconds>(sw.elapsed()));

        if (source.is_err()) {
            auto errorCode = source.err_value();
            return SendFail(ctx, errorCode, "source fail");
        }

        sourceFeature = source.ok_value().feature;
    } while (0);

    auto features = storage->GetFaceFeatures(params.appId);
    auto result = nlohmann::json::array();
    std::priority_queue<SimilarityResult> topKList;
    if (!features.empty()) {
        for (auto &info : features) {
            const float *targetFeature = info.feature.get();
            for (std::size_t idx = 0; idx < info.count; idx++) {
                const float *target = targetFeature + (idx * FACE_FEATURE_LENGHT);
                float similarity = SimilarityCalculator<algorithm::SeetaDefault>::calculate(sourceFeature.data(), target, FACE_FEATURE_LENGHT);
                if (topKList.size() < params.maxUser) {
                    topKList.emplace(info.index, idx, similarity);
                } else if (topKList.top().similarity < similarity) {
                    topKList.pop();
                    topKList.emplace(info.index, idx, similarity);
                }
            }
        }
    }

    while (!topKList.empty()) {
        const auto &value = topKList.top();
        auto user = storage->GetUser(params.appId, value.index, value.offset);
        if (user) {
            float score = std::fmax(0.f, std::fmin(100.f, value.similarity * 100.0));
            auto _user = user.value();
            result.emplace_back(nlohmann::json{
                {"userId", _user.userId},
                {"userInfo", _user.userInfo},
                {"score", score},
            });
        }
        topKList.pop();
    }
    return SendSuccess(ctx, result);
}
};  // namespace handler
};  // namespace server
};  // namespace face
