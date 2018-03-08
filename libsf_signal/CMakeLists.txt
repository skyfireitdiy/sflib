cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(sf_object VERSION 0.1.0 LANGUAGES CXX)

include_directories("${PROJECT_SOURCE_DIR}/../libsf_empty_class/include")

install(FILES "${PROJECT_SOURCE_DIR}/include/sf_object.h" DESTINATION include)