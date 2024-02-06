cmake_minimum_required(VERSION 3.16)
project(face_id_common)

message(STATUS "Configuration: ${PROJECT_NAME}")

# if(APPLE)
#   set(CMAKE_INSTALL_RPATH "@loader_path/../lib")
#   set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
# endif()

file(GLOB SELF_SRCS src/*.hpp src/*.cpp)
add_library(${PROJECT_NAME} SHARED ${SELF_SRCS})
target_include_directories(${PROJECT_NAME} PRIVATE ./src)
if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework CoreFoundation")
endif()


install(TARGETS ${PROJECT_NAME}
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
RUNTIME DESTINATION bin
BUNDLE DESTINATION bin
)