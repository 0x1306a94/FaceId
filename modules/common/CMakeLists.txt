cmake_minimum_required(VERSION 3.16)
project(face_id_common)

message(STATUS "Configuration: ${PROJECT_NAME}")

# if(APPLE)
#   set(CMAKE_INSTALL_RPATH "@loader_path/../lib")
#   set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
# endif()

file(GLOB SELF_SRCS src/*.hpp src/*.cpp)
add_library(${PROJECT_NAME} SHARED ${SELF_SRCS})

install(TARGETS ${PROJECT_NAME}
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
RUNTIME DESTINATION bin
BUNDLE DESTINATION bin
)