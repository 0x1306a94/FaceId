//
//  image_downloader.cpp
//  face_id
//
//  Created by king on 2024/2/19.
//

#include "image_downloader.hpp"

#include "../common/src/AutoBuffer.hpp"

#include <hv/requests.h>

namespace face {
namespace server {
namespace download {
std::size_t image_from_url(std::string_view url, common::AutoBuffer &buffer) {
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
};  // namespace download
}  // namespace server
}  // namespace face
