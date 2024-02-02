cmake_minimum_required(VERSION 3.16)
project(face_id_common)

file(GLOB SELF_SRCS src/*.hpp src/*.cpp)
add_library(${PROJECT_NAME} ${SELF_SRCS})
