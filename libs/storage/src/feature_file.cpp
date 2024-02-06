//
//  feature_file.cpp
//  face_id
//
//  Created by king on 2024/2/2.
//

#include "feature_file.hpp"

#include "spdlog_common.hpp"

#include "feature_file_header.h"

#include "../common/src/byte_order.hpp"

#include <filesystem>

#include <algorithm>
#include <fcntl.h>
#include <memory.h>
#include <mutex>
#include <shared_mutex>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace face {
namespace storage {

namespace fs = std::filesystem;

class FeatureFile::Implement {
  public:
    int m_fd;
    char *m_addr;
    std::uint16_t m_index;
    std::string m_appId;
    fs::path m_path;
    FeatureFileHeader *m_header;
    FeatureItem *m_item;
    mutable std::shared_mutex m_mutex;
    explicit Implement(const std::string &appId, std::uint16_t index, const std::string &dir)
        : m_fd(-1)
        , m_addr(nullptr)
        , m_appId(appId)
        , m_index(index)
        , m_path(dir + "/" + appId + "_" + std::to_string(index))
        , m_header(nullptr)
        , m_item(nullptr) {

        bool exist = access(m_path.c_str(), F_OK) == 0;
        FILE *file = fopen(m_path.c_str(), exist ? "rb+" : "wb+");
        if (file == nullptr) {
            SPDLOG_ERROR("can not open file path: {}", m_path.string());
            return;
        }

        size_t file_size = FACE_FEATURE_FILE_SIZE;
        int fd = fileno(file);
        if (!exist) {
            int res = ftruncate(fd, file_size);
            if (res != 0) {
                SPDLOG_ERROR("can not truncate file path: {}", m_path.string());
                fclose(file);
                return;
            }
        }

        char *addr = (char *)mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (addr == MAP_FAILED) {
            SPDLOG_ERROR("can not mmap file path: {}", m_path.string());
            fclose(file);
            return;
        }

        if (!exist) {
            memset(addr, 0, file_size);
            size_t header_size = sizeof(FeatureFileHeader);
            FeatureFileHeader header;
            header.magic = FACE_FEATURE_FILE_MAGIC;
            memcpy(header.appid, appId.c_str(), std::min(32, (int)appId.length()));
            header.index = m_index;
            header.count = 0;
            memcpy(addr, &header, header_size);
            // 首次创建时手动刷新一次
            msync(addr, file_size, MS_SYNC);
        }
        m_addr = addr;
        m_fd = fd;
        m_header = reinterpret_cast<FeatureFileHeader *>((void *)addr);
        m_item = m_header->features;
#ifndef NDEBUG
        asm("nop");
#endif
    }

    ~Implement() {
        if (m_fd != -1) {
            munmap(m_addr, FACE_FEATURE_FILE_SIZE);
            close(m_fd);
            m_fd = -1;
            m_addr = nullptr;
            m_header = nullptr;
            m_item = nullptr;

            SPDLOG_INFO("close feature file path: {}", m_path.string());
        }
    }

    bool IsFull() const {
        std::shared_lock<std::shared_mutex> lck(m_mutex);
        if (m_header == nullptr) {
            return false;
        }
        uint32_t count = m_header->count;
        return count >= FACE_FEATURE_FILE_MAX_ITEM;
    }

    bool IsValid() const {
        std::shared_lock<std::shared_mutex> lck(m_mutex);
        if (m_header == nullptr) {
            return false;
        }
        return true;
    }

    std::optional<std::uint32_t> AddFeature(const std::vector<float> &feature) {
        std::unique_lock<std::shared_mutex> lck(m_mutex);
        if (m_header == nullptr) {
            SPDLOG_ERROR("feature file does not exist path: {}", m_path.string());
            return std::nullopt;
        }

        uint32_t count = m_header->count;
        if (count >= FACE_FEATURE_FILE_MAX_ITEM) {
            SPDLOG_ERROR("current feature file is fully written path: {}", m_path.string());
            return std::nullopt;
        }

        auto hostOrder = common::NativeOrder();
        float temp[FACE_FEATURE_LENGHT] = {0};
        if (hostOrder == common::ByteOrder::BIG) {
            size_t index = 0;
            for (const auto &value : feature) {
                temp[index++] = common::float_big_to_little(value);
                if (index >= FACE_FEATURE_LENGHT) {
                    break;
                }
            }
        } else {
            size_t copyLen = std::min(static_cast<int>(sizeof(float) * FACE_FEATURE_LENGHT), static_cast<int>(feature.size() * sizeof(float)));
            memcpy(temp, feature.data(), copyLen);
        }

        m_header->count = count + 1;
        FeatureItem *addItem = m_item + count;
        memcpy(addItem->value, temp, sizeof(temp));
        return std::make_optional<std::uint32_t>(count);
    }

    std::optional<std::vector<float>> GetFeature(std::uint32_t offset) {
        std::shared_lock<std::shared_mutex> lck(m_mutex);
        if (m_header == nullptr) {
            return std::nullopt;
        }

        uint32_t count = m_header->count;
        if (offset >= count) {
            return std::nullopt;
        }

        std::vector<float> result(FACE_FEATURE_LENGHT, 0.0f);
        memcpy(result.data(), m_item + (sizeof(FeatureItem) * count), sizeof(FeatureItem));
        auto hostOrder = common::NativeOrder();
        if (hostOrder == common::ByteOrder::BIG) {
            for (size_t idx = 0; idx < FACE_FEATURE_LENGHT; idx++) {
                result[idx] = common::float_little_to_big(result[idx]);
            }
        }
        return std::make_optional<std::vector<float>>(std::move(result));
    }
};

FeatureFile::FeatureFile(const std::string &appId, std::uint16_t index, const std::string &dir)
    : impl(std::make_unique<Implement>(appId, index, dir)) {
}

FeatureFile::~FeatureFile() {
}

std::string FeatureFile::GetAppId() const {
    return impl->m_appId;
}

std::uint16_t FeatureFile::GetIndex() const {
    return impl->m_index;
}

bool FeatureFile::IsFull() const {
    return impl->IsFull();
}

bool FeatureFile::IsValid() const {
    return impl->IsValid();
}

std::optional<std::uint32_t> FeatureFile::AddFeature(const std::vector<float> &feature) {
    return impl->AddFeature(feature);
}

std::optional<std::vector<float>> FeatureFile::GetFeature(std::uint32_t offset) {
    return impl->GetFeature(offset);
}

}  // namespace storage
};  // namespace face
