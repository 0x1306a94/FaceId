//
//  nlohmann_json_reponse_provide.hpp
//  face_id_cli
//
//  Created by king on 2024/2/6.
//

#ifndef face_id_server_nlohmann_json_reponse_provide_hpp
#define face_id_server_nlohmann_json_reponse_provide_hpp

#include <hv/json.hpp>

#include <list>

namespace face {
namespace server {
namespace reqparmas {
struct FaceAddParams;
struct FaceAddResponse;
struct FaceInfoParams;
struct FaceListParams;
struct FaceMatchParams;
struct FaceMatchResponse;
struct FaceSearchParams;
};  // namespace reqparmas
};  // namespace server
};  // namespace face

namespace face {
namespace storage {
class User;
class Application;
class FaceRecord;
};  // namespace storage
};  // namespace face
namespace ns {
void from_json(const nlohmann::json &j, face::server::reqparmas::FaceAddParams &params);
void from_json(const nlohmann::json &j, face::server::reqparmas::FaceInfoParams &params);
void from_json(const nlohmann::json &j, face::server::reqparmas::FaceListParams &params);
void from_json(const nlohmann::json &j, face::server::reqparmas::FaceMatchParams &params);
void from_json(const nlohmann::json &j, face::server::reqparmas::FaceSearchParams &params);

void to_json(nlohmann::json &j, const face::storage::User &user);
void to_json(nlohmann::json &j, const std::list<face::storage::User> &users);

void to_json(nlohmann::json &j, const face::storage::Application &app);
void to_json(nlohmann::json &j, const std::list<face::storage::Application> &apps);

void to_json(nlohmann::json &j, const face::storage::FaceRecord &face);
void to_json(nlohmann::json &j, const std::list<face::storage::FaceRecord> &faces);
void to_json(nlohmann::json &j, const face::server::reqparmas::FaceAddResponse &response);
void to_json(nlohmann::json &j, const face::server::reqparmas::FaceMatchResponse &response);
};  // namespace ns

#endif /* face_id_server_nlohmann_json_reponse_provide_hpp */
