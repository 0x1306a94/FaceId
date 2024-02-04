cmake_minimum_required(VERSION 3.16)
project(face_id_storage)

message(STATUS "Configuration: ${PROJECT_NAME}")

# if(APPLE)
#   set(CMAKE_INSTALL_RPATH "@loader_path/../lib")
#   set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
# endif()

find_library(SPDLOG_LIB spdlog HINTS "${SPDLOG_DIR}/lib" NO_DEFAULT_PATH REQUIRED)
message(STATUS "SPDLOG_LIB: ${SPDLOG_LIB}")

if(APPLE)
  set(FILE_GLOB_OPTION RELATIVE)
endif()

file(GLOB SRC_SRCS src/*.hpp src/*.cpp)
file(GLOB ORM_SRCS src/orm/*.hpp src/orm/*.cpp)

source_group(src FILES ${SRC_SRCS})
source_group(orm FILES ${ORM_SRCS})

add_library(${PROJECT_NAME} SHARED ${SRC_SRCS} ${ORM_SRCS})
target_include_directories(${PROJECT_NAME} PRIVATE 
./src
../common/src
${SPDLOG_DIR}/include)

# target_link_directories(${PROJECT_NAME} PRIVATE 
# ${SPDLOG_DIR}/lib)

target_link_libraries(${PROJECT_NAME} PRIVATE 
${SPDLOG_LIB}
face_id_common)

if(APPLE)

set_target_properties(${PROJECT_NAME} PROPERTIES
  MACOSX_BUNDLE FALSE
  XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
  XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "NO"
  XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Manual"
)

find_library(WCDB_FRAMEWORK_LIBRARY WCDBCpp HINTS "${WCDB_DIR}/" PATH_SUFFIXES ".framework" NO_DEFAULT_PATH REQUIRED)
message(STATUS "WCDB_FRAMEWORK_LIBRARY: ${WCDB_FRAMEWORK_LIBRARY}")
target_link_libraries(${PROJECT_NAME} PRIVATE ${WCDB_FRAMEWORK_LIBRARY})
install(DIRECTORY ${WCDB_FRAMEWORK_LIBRARY} DESTINATION lib)

else()

find_library(WCDB_LIB WCDB HINTS "${WCDB_DIR}/lib" NO_DEFAULT_PATH REQUIRED)
message(STATUS "WCDB_LIB: ${WCDB_LIB}")
target_link_libraries(${PROJECT_NAME} PRIVATE ${WCDB_LIB})
target_include_directories(${PROJECT_NAME} PRIVATE ${WCDB_DIR}/include)
install(FILES ${WCDB_LIB} DESTINATION lib)

endif()

if(NOT APPLE)
  target_compile_options(${PROJECT_NAME} PRIVATE  -Wno-unknown-pragmas -Wno-pragmas -Wno-deprecated -Wno-unused-variable -fpermissive)
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
