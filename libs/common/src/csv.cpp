//
//  csv.cpp
//  face_id
//
//  Created by king on 2024/2/5.
//

#include "csv.hpp"

namespace face {
namespace common {

std::string_view CSVRow::operator[](std::size_t index) const {
    return std::string_view(&m_line[m_data[index] + 1], m_data[index + 1] - (m_data[index] + 1));
}
std::size_t CSVRow::size() const {
    return m_data.size() - 1;
}
void CSVRow::readNextRow(std::istream &str) {
    std::getline(str, m_line);

    m_data.clear();
    m_data.emplace_back(-1);
    std::string::size_type pos = 0;
    while ((pos = m_line.find(',', pos)) != std::string::npos) {
        m_data.emplace_back(pos);
        ++pos;
    }
    // This checks for a trailing comma with no data after it.
    pos = m_line.size();
    m_data.emplace_back(pos);
}

CSVIterator::CSVIterator(std::istream &str)
    : m_str(str.good() ? &str : nullptr) {
    ++(*this);
}

CSVIterator::CSVIterator()
    : m_str(nullptr) {
}

// Pre Increment
CSVIterator &CSVIterator::operator++() {
    if (m_str) {
        if (!((*m_str) >> m_row)) {
            m_str = nullptr;
        }
    }
    return *this;
}
// Post increment
CSVIterator CSVIterator::operator++(int) {
    CSVIterator tmp(*this);
    ++(*this);
    return tmp;
}

CSVRow const &CSVIterator::operator*() const {
    return m_row;
}

CSVRow const *CSVIterator::operator->() const {
    return &m_row;
}

bool CSVIterator::operator==(CSVIterator const &rhs) {
    return ((this == &rhs) || ((this->m_str == nullptr) && (rhs.m_str == nullptr)));
}

bool CSVIterator::operator!=(CSVIterator const &rhs) {
    return !((*this) == rhs);
}

};  // namespace common
};  // namespace face

std::istream &operator>>(std::istream &str, face::common::CSVRow &data) {
    data.readNextRow(str);
    return str;
}
