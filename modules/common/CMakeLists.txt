cmake_minimum_required(VERSION 3.16)
project(face_id_common)

file(GLOB SELF_SRCS src/*.hpp src/*.cpp)
add_library(${PROJECT_NAME} ${SELF_SRCS})

install(TARGETS ${PROJECT_NAME}
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
RUNTIME DESTINATION bin
BUNDLE DESTINATION bin
)