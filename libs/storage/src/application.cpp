//
//  application.cpp
//  face_id
//
//  Created by king on 2024/2/4.
//

#include "application.hpp"

namespace face {
namespace storage {
Application::Application(const std::string &appId, const std::string &name)
    : identifier(0)
    , appId(appId)
    , name(name)
    , createDate(0)
    , updateDate(0) {
}
};  // namespace storage
};  // namespace face
