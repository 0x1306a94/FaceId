cmake_minimum_required(VERSION 3.16)
project(face_id_modules)

include_directories(./include)

add_subdirectory(common)
add_subdirectory(storage)
add_subdirectory(recognizer)
add_subdirectory(server)