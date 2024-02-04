#include "AutoBuffer.hpp"

namespace face {
namespace common {
AutoBuffer::AutoBuffer(size_t malloc_unitsize)
    : m_malloc_unitsize(malloc_unitsize)
    , m_capacity(0)
    , m_size(0) {
}

AutoBuffer::~AutoBuffer() {
    Reset();
}

AutoBuffer::AutoBuffer(AutoBuffer &&other) noexcept {
    if (m_ptr != nullptr) {
        free(m_ptr);
        m_ptr = nullptr;
    }

    m_ptr = other.m_ptr;
    m_size = other.m_size;
    m_malloc_unitsize = other.m_malloc_unitsize;
    m_capacity = other.m_capacity;

    other.m_ptr = nullptr;
    other.m_size = 0;
    other.m_capacity = 0;
}

AutoBuffer &AutoBuffer::operator=(AutoBuffer &&other) {
    if (this == &other) {
        return *this;
    }

    if (m_ptr != nullptr) {
        free(m_ptr);
        m_ptr = nullptr;
    }

    m_ptr = other.m_ptr;
    m_size = other.m_size;
    m_malloc_unitsize = other.m_malloc_unitsize;
    m_capacity = other.m_capacity;

    other.m_ptr = nullptr;
    other.m_size = 0;
    other.m_capacity = 0;

    return *this;
}

void AutoBuffer::Append(const char *data, size_t size) {
    if (size == 0) {
        return;
    }
    size_t new_capacity = m_size + size;
    grow(new_capacity);
    memcpy(m_ptr + m_size, data, size);
    m_size += size;
}

void AutoBuffer::Reset() {
    if (m_ptr != nullptr) {
        free(m_ptr);
        m_ptr = nullptr;
        m_size = 0;
        m_capacity = 0;
    }
}

const char *AutoBuffer::data() const {
    return m_ptr;
}

const size_t AutoBuffer::size() const {
    return m_size;
}

void AutoBuffer::grow(size_t capacity) {
    size_t old_capacity = m_capacity;
    if (capacity > old_capacity) {

        size_t final_capacity = ((capacity + m_malloc_unitsize - 1) / m_malloc_unitsize) * m_malloc_unitsize;
        // std::cout << "old_capacity: " << old_capacity
        //           << " new_capacity: " << capacity
        //           << " final_capacity: " << final_capacity
        //           << std::endl;
        // if (m_ptr == nullptr) {
        //     m_ptr = (char *)malloc(final_capacity);
        //     memset(m_ptr, 0, final_capacity);
        //     m_capacity = final_capacity;
        //     return;
        // }
        char *new_ptr = (char *)realloc(m_ptr, final_capacity);
        if (new_ptr == NULL) {
            free(m_ptr);
            m_ptr = nullptr;
            m_capacity = 0;
            return;
        }
        m_ptr = new_ptr;
        memset(m_ptr + old_capacity, 0, final_capacity - old_capacity);
        m_capacity = final_capacity;
    }
}
}  // namespace common
}  // namespace face
