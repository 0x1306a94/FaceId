//
//  app_file.cpp
//  face_id
//
//  Created by king on 2024/2/2.
//

#include "app_file.hpp"
namespace face {
namespace storage {
AppFile::AppFile(const std::string &appId, const std::string &dataDir)
    : m_appId(appId)
    , m_dataDir(dataDir) {
}

AppFile::~AppFile() {
}

}  // namespace storage
};  // namespace face
