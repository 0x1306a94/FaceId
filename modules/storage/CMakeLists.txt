cmake_minimum_required(VERSION 3.16)
project(face_id_storage)

if(APPLE)
    set(CMAKE_INSTALL_RPATH "@loader_path/../lib") 
endif()

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

file(GLOB SELF_SRCS src/*.hpp src/*.cpp)
add_library(${PROJECT_NAME} SHARED ${SELF_SRCS})
target_include_directories(${PROJECT_NAME} PRIVATE 
./src
../common/src
${SPDLOG_DIR}/include)

target_link_directories(${PROJECT_NAME} PRIVATE 
${SPDLOG_DIR}/lib)

target_link_libraries(${PROJECT_NAME} PRIVATE 
spdlog
face_id_common)

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
