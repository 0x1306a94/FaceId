#ifndef BASE_64_HPP
#define BASE_64_HPP

#include <string>
namespace face {
namespace common {
namespace base64 {
std::string to_base64(std::string const &data);
std::string from_base64(std::string const &data);
}  // namespace base64
}  // namespace common
}  // namespace face

#endif  // BASE_64_HPP
