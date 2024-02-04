cmake_minimum_required(VERSION 3.16)
project(face_id_libs)

include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

add_subdirectory(common)
add_subdirectory(storage)
add_subdirectory(recognizer)