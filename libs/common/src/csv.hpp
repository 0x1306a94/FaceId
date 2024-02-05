//
//  csv.hpp
//  face_id
//
//  Created by king on 2024/2/5.
//

#ifndef face_id_common_csv_hpp
#define face_id_common_csv_hpp

#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

namespace face {
namespace common {
// https://stackoverflow.com/questions/1120140/how-can-i-read-and-parse-csv-files-in-c
class CSVRow {
  public:
    std::string_view operator[](std::size_t index) const;
    std::size_t size() const;
    void readNextRow(std::istream &str);

  private:
    std::string m_line;
    std::vector<int> m_data;
};

class CSVIterator {
  public:
    typedef std::input_iterator_tag iterator_category;
    typedef CSVRow value_type;
    typedef std::size_t difference_type;
    typedef CSVRow *pointer;
    typedef CSVRow &reference;

    CSVIterator(std::istream &str);
    CSVIterator();

    // Pre Increment
    CSVIterator &operator++();
    // Post increment
    CSVIterator operator++(int);
    CSVRow const &operator*() const;
    CSVRow const *operator->() const;

    bool operator==(CSVIterator const &rhs);
    bool operator!=(CSVIterator const &rhs);

  private:
    std::istream *m_str;
    CSVRow m_row;
};

class CSVRange {
    std::istream &stream;
  public:
    CSVRange(std::istream &str)
        : stream(str) {}
    CSVIterator begin() const { return CSVIterator{stream}; }
    CSVIterator end() const { return CSVIterator{}; }
};

};  // namespace common
};  // namespace face

std::istream &operator>>(std::istream &str, face::common::CSVRow &data);

#endif /* face_id_common_csv_hpp */
