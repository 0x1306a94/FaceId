cmake_minimum_required(VERSION 3.16)
project(face_id_tools)

include_directories(${CMAKE_CURRENT_LIST_DIR}/../libs/include)

include(FetchContent)
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG v3.0
)
FetchContent_MakeAvailable(argparse)

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
set(DEFAULT_FACE_ID_DATA_DIR "${CMAKE_INSTALL_PREFIX}/data" CACHE STRING INTERNAL)
file(MAKE_DIRECTORY "${DEFAULT_FACE_ID_DATA_DIR}")

file(INSTALL ${FACE_ID_ROOT_DIR}/sf3.0_models/face_detector.csta DESTINATION ${CMAKE_INSTALL_PREFIX}/sf3.0_models)
file(INSTALL ${FACE_ID_ROOT_DIR}/sf3.0_models/face_landmarker_pts5.csta DESTINATION ${CMAKE_INSTALL_PREFIX}/sf3.0_models)
file(INSTALL ${FACE_ID_ROOT_DIR}/sf3.0_models/face_recognizer.csta DESTINATION ${CMAKE_INSTALL_PREFIX}/sf3.0_models)
set(DEFAULT_FACE_ID_MODELE_DIR "${CMAKE_INSTALL_PREFIX}/sf3.0_models" CACHE STRING INTERNAL)

configure_file(${CMAKE_CURRENT_LIST_DIR}/config.yaml.in ${CMAKE_INSTALL_PREFIX}/config/config.yaml @ONLY)

add_subdirectory(server)
add_subdirectory(cli)