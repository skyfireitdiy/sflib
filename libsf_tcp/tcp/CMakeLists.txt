cmake_minimum_required(VERSION 3.9)
project(sf_tcp)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/../build")
add_library(sf_tcp SHARED src/tcpserver.cpp include/tcpserver.h src/tcpclient.cpp include/tcpclient.h include/tcputils.h)

target_link_libraries(sf_tcp ws2_32)