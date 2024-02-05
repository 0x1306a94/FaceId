//
//  application.cpp
//  face_id
//
//  Created by king on 2024/2/4.
//

#include "application.hpp"

#include "./orm/application_orm.hpp"
namespace face {
namespace storage {
Application::Application(const std::string &appId, const std::string &name)
    : appId(appId)
    , name(name)
    , createDate(0)
    , updateDate(0) {
}

Application::Application(const ApplicationORM &source)
    : appId(source.appId)
    , name(source.name)
    , createDate(source.createDate)
    , updateDate(source.updateDate) {
}

};  // namespace storage
};  // namespace face
