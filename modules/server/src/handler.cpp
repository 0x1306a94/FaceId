

#include "config.hpp"

#include "SimilarityCalculator.hpp"
#include "handler.hpp"
#include "server_context.hpp"
#include "spdlog_common.hpp"

#include "../common/src/AutoBuffer.hpp"
#include "../common/src/base64.hpp"
#include "../common/src/util.hpp"
#include "../recognizer/src/FeatureEngine.hpp"
#include "../recognizer/src/FeatureEnginePool.hpp"
#include "../storage/src/Storage.hpp"

#include <hv/HttpMessage.h>
#include <hv/HttpService.h>
#include <hv/hurl.h>
#include <hv/json.hpp>
#include <hv/requests.h>

#include <spdlog/fmt/chrono.h>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>

#include <cstdint>
#include <exception>
#include <sstream>
#include <thread>
#include <vector>

using std::chrono::duration_cast;
using std::chrono::milliseconds;

namespace face {
namespace server {
const char *FACE_FEATURE_ENV_HEADER_NAME = "FACE_FEATURE_ENV";
static std::shared_ptr<recognizer::FeatureEnginePool> g_FeatureEnginePool = nullptr;

size_t fetch_image_form_url(const std::string &url, common::AutoBuffer &buffer) {

    requests::Request req(new HttpRequest);
    req->method = HTTP_GET;
    req->url = url;
    req->timeout = 3600;  // 1h
    size_t content_length = 0;
    size_t received_bytes = 0;
    req->http_cb = [&buffer, &content_length, &received_bytes](HttpMessage *resp, http_parser_state state, const char *data, size_t size) {
        if (!resp->headers["Location"].empty()) return;
        if (state == HP_HEADERS_COMPLETE) {
            content_length = hv::from_string<size_t>(resp->GetHeader("Content-Length"));
        } else if (state == HP_BODY) {
            if (data && size) {
                buffer.Append(data, size);
                received_bytes += size;
            }
        }
    };

    auto resp = requests::request(req);
    if (resp == NULL || resp->status_code != 200) {
        return 0;
    }
    return received_bytes;
}

bool envCheck(const HttpContextPtr &ctx) {
    std::string env = ctx->request->GetHeader(FACE_FEATURE_ENV_HEADER_NAME);
    if (env.empty()) {
        return false;
    }

    //    if (!face::Database::ValidEnv(env)) {
    //        return false;
    //    }

    return true;
}

#define ENV_CHECK()                                                         \
    std::string env_header;                                                 \
    do {                                                                    \
        if (!envCheck(ctx)) {                                               \
            SPDLOG_ERROR("请求头 FACE_FEATURE_ENV 错误或者缺少");           \
            return sendFail(ctx, -102, "请检查 FACE_FEATURE_ENV Header");   \
        }                                                                   \
        env_header = ctx->request->GetHeader(FACE_FEATURE_ENV_HEADER_NAME); \
    } while (0)

void Handler::registerFeatureEnginePool(std::shared_ptr<recognizer::FeatureEnginePool> engine_pool) {
    g_FeatureEnginePool = engine_pool;
}

int Handler::extractFeatureFromFile(const HttpContextPtr &ctx) {

    ENV_CHECK();

    std::string value = ctx->get("index");
    bool returnFeature = ctx->get<bool>("return_feature");
    bool addDatabase = ctx->get<bool>("add_database");

    if (value.empty()) {
        SPDLOG_ERROR("index params empty");
        return sendFail(ctx, -103, "index params empty");
    }

    int64_t index = -1;
    //    try {
    //        index = std::stoll(value);
    //        if (index < face::INDEX_START) {
    //            SPDLOG_ERROR("index must be greater than or equal to {}", face::INDEX_START);
    //            return sendFail(ctx, -103, "index must be greater than or equal to 0");
    //        }
    //    } catch (const std::exception &e) {
    //        SPDLOG_ERROR("exception: {}", e.what());
    //        return sendFail(ctx, -1, e.what());
    //    }

    auto form = ctx->form();
    auto iter = form.find("file");
    if (iter == form.end()) {
        SPDLOG_ERROR("no file");
        return sendFail(ctx, -102, "no file");
    }

    SPDLOG_INFO("extract feature index: {} env: {}", index, env_header);

    const auto &formdata = iter->second;

    nlohmann::json json;

    auto fr = g_FeatureEnginePool->Acquire();

    spdlog::stopwatch sw;
    face::common::Result<recognizer::FeatureEngine::Result, recognizer::FeatureEngine::ErrorCode> result = fr->ExtractFeatureFromData(formdata.content);

    g_FeatureEnginePool->Release(std::move(fr));

    SPDLOG_INFO("人脸特征提取 index: {} env: {} elapsed: {}", index, env_header, duration_cast<milliseconds>(sw.elapsed()));
    if (result.is_err()) {
        json["code"] = result.err_value();
    } else {
        auto value = result.ok_value();
        auto added = false;

        json["code"] = "0";
        if (returnFeature) {
            json["result"] = nlohmann::json{{"feature", value.feature}, {"added", added}};
        } else {
            json["result"] = nlohmann::json{{"added", added}};
        }
    }

    return ctx->send(json.dump());
}

int Handler::extractFeatureFromBase64(const HttpContextPtr &ctx) {
    ENV_CHECK();
    std::string value = ctx->get("index");
    bool returnFeature = ctx->get<bool>("return_feature");
    bool addDatabase = ctx->get<bool>("add_database");

    if (value.empty()) {
        SPDLOG_ERROR("index params empty");
        return sendFail(ctx, -103, "index params empty");
    }

    int64_t index = -1;
    //    try {
    //        index = std::stoll(value);
    //        if (index < face::INDEX_START) {
    //            SPDLOG_ERROR("index must be greater than or equal to {}", face::INDEX_START);
    //            return sendFail(ctx, -103, "index must be greater than or equal to 0");
    //        }
    //    } catch (const std::exception &e) {
    //        SPDLOG_ERROR("exception: {}", e.what());
    //        return sendFail(ctx, -1, e.what());
    //    }

    auto base64_img = ctx->get("image");

    if (base64_img.empty()) {
        return sendFail(ctx, -1, "image params empty");
    }

    SPDLOG_INFO("extract feature index: {} env: {}", index, env_header);

    nlohmann::json json;

    auto fr = g_FeatureEnginePool->Acquire();

    spdlog::stopwatch sw;
    auto result = fr->ExtractFeatureFromBase64(base64_img);

    g_FeatureEnginePool->Release(std::move(fr));

    SPDLOG_INFO("人脸特征提取 base64 index: {} env: {} elapsed: {}", index, env_header, duration_cast<milliseconds>(sw.elapsed()));
    if (result.is_err()) {
        json["code"] = result.err_value();
    } else {
        auto value = result.ok_value();
        auto added = false;

        json["code"] = "0";
        if (returnFeature) {
            json["result"] = nlohmann::json{{"feature", value.feature}, {"added", added}};
        } else {
            json["result"] = nlohmann::json{{"added", added}};
        }
    }

    return ctx->send(json.dump());
}

int Handler::extractFeatureFromURL(const HttpContextPtr &ctx) {

    ENV_CHECK();

    std::string value = ctx->get("index");
    bool returnFeature = ctx->get<bool>("return_feature");
    bool addDatabase = ctx->get<bool>("add_database");
    if (value.empty()) {
        SPDLOG_ERROR("index params empty");
        return sendFail(ctx, -103, "index params empty");
    }

    int64_t index = -1;
    //    try {
    //        index = std::stoll(value);
    //        if (index < face::INDEX_START) {
    //            SPDLOG_ERROR("index must be greater than or equal to {}", face::INDEX_START);
    //            return sendFail(ctx, -103, "index must be greater than or equal to 0");
    //        }
    //    } catch (const std::exception &e) {
    //        SPDLOG_ERROR("exception: {}", e.what());
    //        return sendFail(ctx, -1, e.what());
    //    }

    SPDLOG_INFO("extract feature index: {} env: {}", index, env_header);

    auto imageURL = ctx->get("url");
    if (imageURL.empty()) {
        return sendFail(ctx, -1, "url params empty");
    }

    HUrl url;
    if (!url.parse(imageURL)) {
        SPDLOG_ERROR("url 非法: index: {} env: {} url: {}", index, env_header, imageURL);
        return sendFail(ctx, -100, "URL非法");
    }

    common::AutoBuffer buffer;
    {
        spdlog::stopwatch sw;
        size_t size = fetch_image_form_url(imageURL, buffer);
        SPDLOG_INFO("加载网络图片: index: {} env: {} url: {} elapsed: {}", index, env_header, imageURL, duration_cast<milliseconds>(sw.elapsed()));
        if (size == 0) {
            SPDLOG_ERROR("url 无法加载: index: {} env: {} url: {}", index, env_header, imageURL);
            return sendFail(ctx, -101, "URL无法加载");
        }
    }

    nlohmann::json json;

    auto fr = g_FeatureEnginePool->Acquire();

    spdlog::stopwatch sw;
    auto result = fr->ExtractFeatureFromData(std::move(buffer));

    g_FeatureEnginePool->Release(std::move(fr));

    SPDLOG_INFO("人脸特征提取 index: {} env: {} elapsed: {}", index, env_header, duration_cast<milliseconds>(sw.elapsed()));
    if (result.is_err()) {
        std::stringstream ss;
        json["code"] = result.err_value();
    } else {
        auto value = result.ok_value();
        auto added = false;

        json["code"] = "0";
        if (returnFeature) {
            json["result"] = nlohmann::json{{"feature", value.feature}, {"added", added}};
        } else {
            json["result"] = nlohmann::json{{"added", added}};
        }
    }

    return ctx->send(json.dump());
}

int Handler::compareFeature(const HttpContextPtr &ctx) {

    ENV_CHECK();

    auto params1 = ctx->get("feature1");
    auto params2 = ctx->get("feature2");
    if (params1.empty()) {
        SPDLOG_ERROR("feature1 params empty");
        return sendFail(ctx, -1, "feature1 params empty");
    }

    if (params2.empty()) {
        SPDLOG_ERROR("feature2 params empty");
        return sendFail(ctx, -1, "feature2 params empty");
    }

    //    std::vector<float> feature1(face::FEATURE_LENGHT);
    //    do {
    //        std::string decode = base64::from_base64(params1);
    //        if (decode.size() != (face::FEATURE_LENGHT * 4)) {
    //            SPDLOG_ERROR("feature1 params empty");
    //            return sendFail(ctx, -1, "feature1 params empty");
    //        }
    //        std::vector<float> temp(face::FEATURE_LENGHT);
    //        std::memcpy(reinterpret_cast<char *>(temp.data()), decode.data(), decode.size());
    //        for (size_t i = 0; i < face::FEATURE_LENGHT; i++) {
    //            feature1[i] = face::utils::float_big_to_little(temp[i]);
    //        }
    //
    //    } while (0);
    //
    //    std::vector<float> feature2(face::FEATURE_LENGHT);
    //    do {
    //        std::string decode = base64::from_base64(params2);
    //        if (decode.size() != (face::FEATURE_LENGHT * 4)) {
    //            SPDLOG_ERROR("feature2 params empty");
    //            return sendFail(ctx, -1, "feature2 params empty");
    //        }
    //        std::vector<float> temp(face::FEATURE_LENGHT);
    //        std::memcpy(reinterpret_cast<char *>(temp.data()), decode.data(), decode.size());
    //        for (size_t i = 0; i < face::FEATURE_LENGHT; i++) {
    //            feature2[i] = face::utils::float_big_to_little(temp[i]);
    //        }
    //    } while (0);
    //
    //    //    auto fr = g_FeatureEnginePool->Acquire();
    //    //    float similarity = fr->CalculateSimilarity(feature1.data(), feature2.data());
    //    //    g_FeatureEnginePool->Release(std::move(fr));
    //
    //    float similarity = SimilarityCalculator<algorithm::SeetaDefault>::calculate(feature1.data(), feature2.data(), feature1.size());
    //    nlohmann::json json;
    //    json["code"] = "0";
    //    json["result"] = similarity;
    //    return ctx->send(json.dump());

    return 0;
}

int Handler::queryFeature(const HttpContextPtr &ctx) {
    ENV_CHECK();
    std::string value = ctx->get("index");
    bool encode = ctx->get<bool>("encode");
    if (value.empty()) {
        SPDLOG_ERROR("index params empty");
        return sendFail(ctx, -1, "index params empty");
    }

    do {
        auto storage = Context::Current()->GetStorage();
        auto feature = storage->QueryFeature(value);
        nlohmann::json response;
        response["code"] = "0";
        response["result"] = feature;
        return ctx->send(response.dump());
    } while (0);

    int64_t index = -1;

    //    try {
    //        index = std::stoll(value);
    //        if (index < face::INDEX_START) {
    //            SPDLOG_ERROR("index must be greater than or equal to {}", face::INDEX_START);
    //            return sendFail(ctx, -103, "index must be greater than or equal to 0");
    //        }
    //    } catch (const std::exception &e) {
    //        SPDLOG_ERROR("exception: {}", e.what());
    //        return sendFail(ctx, -1, e.what());
    //    }

    size_t FeatureLenght = 1024;  //face::Database::GetFeatureLenght();
    std::vector<float> feature(FeatureLenght);
    bool matched = false;  // g_database->Get(index, feature.data(), env_header);
    SPDLOG_INFO("查找特征: index: {} env: {}", index, env_header);

    nlohmann::json response;
    response["code"] = "0";
    if (matched) {
        if (encode) {
            std::vector<float> big_feature(FeatureLenght);
            for (size_t i = 0; i < FeatureLenght; i++) {
                float f = feature[i];
                big_feature[i] = common::utils::float_little_to_big(f);
            }
            std::string content(reinterpret_cast<const char *>(big_feature.data()), (sizeof(float) * big_feature.size()));
            std::string base64_str = common::base64::to_base64(content);
            response["result"] = base64_str;
        } else {
            response["result"] = feature;
        }
    }
    return ctx->send(response.dump());
}

int Handler::batchQueryFeature(const HttpContextPtr &ctx) {
    ENV_CHECK();

    if (!ctx->is(APPLICATION_JSON)) {
        SPDLOG_ERROR("不是json请求");
        return sendFail(ctx, -1, "Use json format for the request");
    }
    auto json = ctx->request->GetJson();

    std::int8_t encode = 0;
    if (json.contains("encode") && json["encode"].is_number_integer()) {
        encode = json["encode"].get<std::int8_t>();
    }

    std::vector<int64_t> indexs;
    if (json.contains("indexs") && json["indexs"].is_array()) {
        indexs = json["indexs"].get<std::vector<int64_t>>();
    }

    nlohmann::json results;

    for (auto index : indexs) {
        size_t FeatureLenght = 1024;  //face::Database::GetFeatureLenght();
        std::vector<float> feature(FeatureLenght);
        bool matched = false;  //g_database->Get(index, feature.data(), env_header);
        SPDLOG_INFO("查找特征: index: {} env: {}", index, env_header);
        if (matched) {
            if (encode == 1) {
                std::vector<float> big_feature(FeatureLenght);
                for (size_t i = 0; i < FeatureLenght; i++) {
                    float f = feature[i];
                    big_feature[i] = common::utils::float_little_to_big(f);
                }
                std::string content(reinterpret_cast<const char *>(big_feature.data()), (sizeof(float) * big_feature.size()));
                std::string base64_str = common::base64::to_base64(content);
                results[std::to_string(index)] = base64_str;
            } else {
                results[std::to_string(index)] = feature;
            }
        }
    }

    nlohmann::json response;
    response["code"] = "0";
    if (!results.empty()) {
        response["result"] = results;
    }
    return ctx->send(response.dump());
}

int Handler::sendFail(const HttpContextPtr &ctx, int code, const std::string &msg) {
    nlohmann::json json;
    json["code"] = code;
    json["msg"] = msg;
    return ctx->send(json.dump());
}
}  // namespace server
}  // namespace face
