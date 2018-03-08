cmake_minimum_required(VERSION 3.9)
project(test_tcpclient)

set(CMAKE_CXX_STANDARD 11)

include_directories("${PROJECT_SOURCE_DIR}/../tcp/include")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/../build")
link_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_executable(test_tcpclient main.cpp)

target_link_libraries(test_tcpclient sf_tcp)