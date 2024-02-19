//
//  image_downloader.hpp
//  face_id
//
//  Created by king on 2024/2/19.
//

#ifndef __face_id_server_image_downloader_hpp__
#define __face_id_server_image_downloader_hpp__

#include <string_view>

namespace face {
namespace common {
class AutoBuffer;
};
};  // namespace face

namespace face {
namespace server {
namespace download {
std::size_t image_from_url(std::string_view url, common::AutoBuffer &buffer);
};
}  // namespace server
}  // namespace face
#endif
