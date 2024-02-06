//
//  nlohmann_json_reponse_provide.cpp
//  face_id_cli
//
//  Created by king on 2024/2/6.
//

#include "nlohmann_json_reponse_provide.hpp"

#include "../storage/src/application.hpp"
#include "../storage/src/face_record.hpp"
#include "../storage/src/user.hpp"

#include "face_req_params.hpp"

namespace ns {
void from_json(const nlohmann::json &j, face::server::reqparmas::FaceAddParams &params) {
    params.appId = j["appId"].get<std::string>();
    params.userId = j["userId"].get<std::string>();
    params.image = j["image"].get<std::string>();
    if (j.contains("userInfo")) {
        params.userInfo = j["userInfo"].get<std::string>();
    }
}

void from_json(const nlohmann::json &j, face::server::reqparmas::FaceListParams &params) {
    params.appId = j["appId"].get<std::string>();
    if (j.contains("userId")) {
        params.userId = j["userId"].get<std::string>();
    }
    if (j.contains("count")) {
        params.count = j["count"].get<std::int64_t>();
    }
}

void to_json(nlohmann::json &j, const face::storage::User &user) {
    j = nlohmann::json{
        {"appId", user.appId},
        {"userId", user.userId},
        {"userInfo", user.userInfo},
        {"createDate", user.createDate},
        {"updateDate", user.updateDate},
    };
}

void to_json(nlohmann::json &j, const std::list<face::storage::User> &users) {
    j = nlohmann::json::array();
    for (const auto &user : users) {
        nlohmann::json obj;
        to_json(obj, user);
        j.push_back(obj);
    }
}

void to_json(nlohmann::json &j, const face::storage::Application &app) {
    j = nlohmann::json{
        {"appId", app.appId},
        {"name", app.name},
        {"createDate", app.createDate},
        {"updateDate", app.updateDate},
    };
}

void to_json(nlohmann::json &j, const std::list<face::storage::Application> &apps) {
    j = nlohmann::json::array();
    for (const auto &app : apps) {
        nlohmann::json obj;
        to_json(obj, app);
        j.push_back(obj);
    }
}

void to_json(nlohmann::json &j, const face::storage::FaceRecord &face) {
    j = nlohmann::json{
        {"appId", face.appId},
        {"userId", face.userId},
        {"fileIndex", face.fileIndex},
        {"fileOffset", face.fileOffset},
        {"createDate", face.createDate},
        {"updateDate", face.updateDate},
    };
}

void to_json(nlohmann::json &j, const std::list<face::storage::FaceRecord> &faces) {
    j = nlohmann::json::array();
    for (const auto &face : faces) {
        nlohmann::json obj;
        to_json(obj, face);
        j.push_back(obj);
    }
}
};  // namespace ns
