cmake_minimum_required(VERSION 3.16)
project(face_id_server)

message(STATUS "Configuration: ${PROJECT_NAME}")

if(APPLE)
  set(CMAKE_INSTALL_RPATH "@executable_path/../lib")
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()

find_library(SPDLOG_LIB spdlog HINTS "${SPDLOG_DIR}/lib" NO_DEFAULT_PATH REQUIRED)
message(STATUS "SPDLOG_LIB: ${SPDLOG_LIB}")

find_library(YAML_CPP_LIB yaml-cpp HINTS "${YAML_CPP_DIR}/lib" NO_DEFAULT_PATH REQUIRED)
message(STATUS "YAML_CPP_LIB: ${YAML_CPP_LIB}")

find_library(HV_LIB hv HINTS "${LIBHV_DIR}/lib" NO_DEFAULT_PATH REQUIRED)
message(STATUS "HV_LIB: ${HV_LIB}")

# 默认 http server 端口
set(DEFAULT_HTTP_SRVER_PORT "6666" CACHE STRING INTERNAL)
# 默认日志文件目录
if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  set(DEFAULT_HTTP_SRVER_LOG_DIR "${CMAKE_INSTALL_PREFIX}/log/face_id_server" CACHE STRING INTERNAL)
else()
  set(DEFAULT_HTTP_SRVER_LOG_DIR "/var/log/face_id_server" CACHE STRING INTERNAL)
endif()
# 创建日志录
file(MAKE_DIRECTORY "${DEFAULT_HTTP_SRVER_LOG_DIR}")
set(DEFAULT_FACE_DATA_DIR "${CMAKE_INSTALL_PREFIX}/data" CACHE STRING INTERNAL)
file(MAKE_DIRECTORY "${DEFAULT_FACE_DATA_DIR}")

file(INSTALL ${CMAKE_CURRENT_SOURCE_DIR}/../../sf3.0_models/face_detector.csta DESTINATION ${CMAKE_INSTALL_PREFIX}/sf3.0_models)
file(INSTALL ${CMAKE_CURRENT_SOURCE_DIR}/../../sf3.0_models/face_landmarker_pts5.csta DESTINATION ${CMAKE_INSTALL_PREFIX}/sf3.0_models)
file(INSTALL ${CMAKE_CURRENT_SOURCE_DIR}/../../sf3.0_models/face_recognizer.csta DESTINATION ${CMAKE_INSTALL_PREFIX}/sf3.0_models)
set(DEFAULT_FACE_MODELE_DIR "${CMAKE_INSTALL_PREFIX}/sf3.0_models" CACHE STRING INTERNAL)

configure_file(${CMAKE_CURRENT_LIST_DIR}/config.yaml.in ${CMAKE_INSTALL_PREFIX}/config/config.yaml @ONLY)

file(GLOB SELF_SRCS src/*.hpp src/*.cpp)
add_executable(${PROJECT_NAME} ${SELF_SRCS})
target_include_directories(${PROJECT_NAME} PRIVATE 
./src
../common/src
../recognizer/src
../storage/src
${LIBHV_DIR}/include
${YAML_CPP_DIR}/include
${SPDLOG_DIR}/include)

# target_link_directories(${PROJECT_NAME} PRIVATE 
# ${SEETA_FACE_DIR}/lib64 
# ${LIBHV_DIR}/lib 
# ${YAML_CPP_DIR}/lib
# ${SPDLOG_DIR}/lib)

target_link_libraries(${PROJECT_NAME} PRIVATE 
${YAML_CPP_LIB}
${HV_LIB}
${SPDLOG_LIB}
face_id_common
face_id_recognizer
face_id_storage)

if(APPLE)
add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_INSTALL_NAME_TOOL} -change libhv.dylib @rpath/libhv.dylib $<TARGET_FILE:${PROJECT_NAME}>
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Fixing library dependencies with ${CMAKE_INSTALL_NAME_TOOL}"
)

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

install(FILES
${LIBHV_DIR}/lib/libhv${CMAKE_SHARED_LIBRARY_SUFFIX}
DESTINATION lib
)

# install(FILES 
# ${CMAKE_CURRENT_SOURCE_DIR}/../../sf3.0_models/face_detector.csta
# ${CMAKE_CURRENT_SOURCE_DIR}/../../sf3.0_models/face_landmarker_pts5.csta
# ${CMAKE_CURRENT_SOURCE_DIR}/../../sf3.0_models/face_recognizer.csta
# DESTINATION sf3.0_models
# )

# uninstall target
if(NOT TARGET uninstall)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()
