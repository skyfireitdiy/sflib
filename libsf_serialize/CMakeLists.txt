cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(sf_serialize VERSION 0.1.0 LANGUAGES CXX)

include_directories("${PROJECT_SOURCE_DIR}/include")

add_library(sf_serialize SHARED src/sf_serialize.cpp)

add_library(sf_serialize_static STATIC src/sf_serialize.cpp)

install(FILES "${PROJECT_SOURCE_DIR}/include/sf_serialize.h" DESTINATION include)

install(TARGETS sf_serialize sf_serialize_static
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)