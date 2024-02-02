//
//  Storage.cpp
//  face_id
//
//  Created by king on 2024/2/2.
//

#include "Storage.hpp"
#include "app_file.hpp"

namespace face {
namespace storage {
Storage::Storage(const std::string &appId, const std::string &dataDir)
    : m_app_file(std::make_unique<AppFile>(appId, dataDir)) {
}

Storage::~Storage() {
}
}  // namespace storage
}  // namespace face
