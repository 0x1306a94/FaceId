cmake_minimum_required(VERSION 3.16)
project(face_id_cli)

message(STATUS "Configuration: ${PROJECT_NAME}")

if(APPLE)
  set(CMAKE_INSTALL_RPATH "@executable_path/../lib")
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()

find_library(SPDLOG_LIB spdlog HINTS "${SPDLOG_DIR}/lib" NO_DEFAULT_PATH REQUIRED)
message(STATUS "SPDLOG_LIB: ${SPDLOG_LIB}")

find_library(YAML_CPP_LIB yaml-cpp HINTS "${YAML_CPP_DIR}/lib" NO_DEFAULT_PATH REQUIRED)
message(STATUS "YAML_CPP_LIB: ${YAML_CPP_LIB}")

configure_file(${CMAKE_CURRENT_LIST_DIR}/version.hpp.in autogen/include/${PROJECT_NAME}/version.hpp @ONLY)

file(GLOB SELF_SRCS src/*.hpp src/*.cpp)
add_executable(${PROJECT_NAME} ${SELF_SRCS})
target_include_directories(${PROJECT_NAME} PRIVATE 
./src
${FACE_ID_ROOT_DIR}/libs/common/src
${FACE_ID_ROOT_DIR}/libs/storage/src
${FACE_ID_ROOT_DIR}/libs/include
${SPDLOG_DIR}/include
${YAML_CPP_DIR}/include
${CMAKE_CURRENT_BINARY_DIR}/autogen/include)

target_link_libraries(${PROJECT_NAME} PRIVATE
${SPDLOG_LIB}
${YAML_CPP_LIB}
face_id_common
face_id_storage
face_id_recognizer
argparse)

if(APPLE)
set_target_properties(${PROJECT_NAME} PROPERTIES
  MACOSX_BUNDLE FALSE
  XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
  XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "NO"
  XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Manual"
)
endif()

install(TARGETS ${PROJECT_NAME}
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
RUNTIME DESTINATION bin
BUNDLE DESTINATION bin
)

# uninstall target
if(NOT TARGET uninstall)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()
