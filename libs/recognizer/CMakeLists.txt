cmake_minimum_required(VERSION 3.16)
project(face_id_recognizer)

message(STATUS "Configuration: ${PROJECT_NAME}")

find_package(OpenCV REQUIRED COMPONENTS core imgproc imgcodecs CONFIG)
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

# if(APPLE)
#   set(CMAKE_INSTALL_RPATH "@loader_path/../lib")
#   set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
# endif()

find_library(SEETA_FACE_AUTHORIZE_LIB SeetaAuthorize HINTS "${SEETA_FACE_DIR}/lib64" NO_DEFAULT_PATH REQUIRED)
find_library(SEETA_FACE_TENNIS_LIB tennis HINTS "${SEETA_FACE_DIR}/lib64" NO_DEFAULT_PATH REQUIRED)
find_library(SEETA_FACE_DETECTOR_LIB SeetaFaceDetector600 HINTS "${SEETA_FACE_DIR}/lib64" NO_DEFAULT_PATH REQUIRED)
find_library(SEETA_FACE_LANDMARKER_LIB SeetaFaceLandmarker600 HINTS "${SEETA_FACE_DIR}/lib64" NO_DEFAULT_PATH REQUIRED)
find_library(SEETA_FACE_RECOGNIZER_LIB SeetaFaceRecognizer610 HINTS "${SEETA_FACE_DIR}/lib64" NO_DEFAULT_PATH REQUIRED)
list(APPEND SEETA_FACE_LIBS ${SEETA_FACE_AUTHORIZE_LIB} ${SEETA_FACE_TENNIS_LIB} ${SEETA_FACE_DETECTOR_LIB} ${SEETA_FACE_LANDMARKER_LIB} ${SEETA_FACE_RECOGNIZER_LIB})
message(STATUS "SEETA_FACE_LIBS: ${SEETA_FACE_LIBS}")
install(FILES ${SEETA_FACE_LIBS} DESTINATION lib)

find_library(SPDLOG_LIB spdlog HINTS "${SPDLOG_DIR}/lib" NO_DEFAULT_PATH REQUIRED)
message(STATUS "SPDLOG_LIB: ${SPDLOG_LIB}")

file(GLOB SELF_SRCS src/*.hpp src/*.cpp)
add_library(${PROJECT_NAME} SHARED ${SELF_SRCS})
target_include_directories(${PROJECT_NAME} PRIVATE 
./src
../common/src
${SEETA_FACE_DIR}/include 
${SPDLOG_DIR}/include
${OpenCV_INCLUDE_DIRS})

# target_link_directories(${PROJECT_NAME} PRIVATE 
# ${SEETA_FACE_DIR}/lib64 
# ${SPDLOG_DIR}/lib)

target_link_libraries(${PROJECT_NAME} PRIVATE 
${OpenCV_LIBS} 
${SEETA_FACE_LIBS}
${SPDLOG_LIB}
face_id_common)

if(APPLE)
add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_INSTALL_NAME_TOOL} -change libSeetaAuthorize.dylib @rpath/libSeetaAuthorize.dylib $<TARGET_FILE:${PROJECT_NAME}>
    COMMAND ${CMAKE_INSTALL_NAME_TOOL} -change libtennis.dylib @rpath/libtennis.dylib $<TARGET_FILE:${PROJECT_NAME}>
    COMMAND ${CMAKE_INSTALL_NAME_TOOL} -change libSeetaFaceDetector600.dylib @rpath/libSeetaFaceDetector600.dylib $<TARGET_FILE:${PROJECT_NAME}>
    COMMAND ${CMAKE_INSTALL_NAME_TOOL} -change libSeetaFaceLandmarker600.dylib @rpath/libSeetaFaceLandmarker600.dylib $<TARGET_FILE:${PROJECT_NAME}>
    COMMAND ${CMAKE_INSTALL_NAME_TOOL} -change libSeetaFaceRecognizer610.dylib @rpath/libSeetaFaceRecognizer610.dylib $<TARGET_FILE:${PROJECT_NAME}>
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

# install(FILES 
# ${SEETA_FACE_DIR}/lib64/libSeetaAuthorize${CMAKE_SHARED_LIBRARY_SUFFIX}
# ${SEETA_FACE_DIR}/lib64/libtennis${CMAKE_SHARED_LIBRARY_SUFFIX}
# ${SEETA_FACE_DIR}/lib64/libSeetaFaceDetector600${CMAKE_SHARED_LIBRARY_SUFFIX}
# ${SEETA_FACE_DIR}/lib64/libSeetaFaceLandmarker600${CMAKE_SHARED_LIBRARY_SUFFIX}
# ${SEETA_FACE_DIR}/lib64/libSeetaFaceRecognizer610${CMAKE_SHARED_LIBRARY_SUFFIX}
# ${SEETA_FACE_DIR}/lib64/libSeetaGenderPredictor600${CMAKE_SHARED_LIBRARY_SUFFIX}
# DESTINATION lib
# )

if(EXISTS ${SEETA_FACE_DIR}/lib64/libtennis_haswell${CMAKE_SHARED_LIBRARY_SUFFIX})
    install(FILES 
      ${SEETA_FACE_DIR}/lib64/libtennis_haswell${CMAKE_SHARED_LIBRARY_SUFFIX}
      DESTINATION lib
    )
endif()
  
if(EXISTS ${SEETA_FACE_DIR}/lib64/libtennis_sandy_bridge${CMAKE_SHARED_LIBRARY_SUFFIX})
    install(FILES 
      ${SEETA_FACE_DIR}/lib64/libtennis_sandy_bridge${CMAKE_SHARED_LIBRARY_SUFFIX}
      DESTINATION lib
    )
endif()

if(EXISTS ${SEETA_FACE_DIR}/lib64/libtennis_pentium${CMAKE_SHARED_LIBRARY_SUFFIX})
    install(FILES 
      ${SEETA_FACE_DIR}/lib64/libtennis_pentium${CMAKE_SHARED_LIBRARY_SUFFIX}
      DESTINATION lib
    )
endif()

# uninstall target
if(NOT TARGET uninstall)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()
