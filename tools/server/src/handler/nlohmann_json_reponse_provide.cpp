//
//  nlohmann_json_reponse_provide.cpp
//  face_id_cli
//
//  Created by king on 2024/2/6.
//

#include "nlohmann_json_reponse_provide.hpp"

#include "../storage/src/application.hpp"
#include "../storage/src/user.hpp"

namespace ns {
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
};  // namespace ns
