cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(sf_tcp VERSION 0.1.0 LANGUAGES CXX)

include_directories("${PROJECT_SOURCE_DIR}/include")

install(FILES "${PROJECT_SOURCE_DIR}/include/sf_tcpclient.h" DESTINATION include)
install(FILES "${PROJECT_SOURCE_DIR}/include/sf_tcpserver.h" DESTINATION include)
install(FILES "${PROJECT_SOURCE_DIR}/include/sf_tcputils.h" DESTINATION include)

add_library(sf_tcp SHARED src/sf_tcpclient.cpp src/sf_tcpserver.cpp)

add_library(sf_tcp_static STATIC src/sf_tcpclient.cpp src/sf_tcpserver.cpp)

target_link_libraries(sf_tcp ws2_32)
target_link_libraries(sf_tcp_static ws2_32)


install(TARGETS sf_tcp sf_tcp_static
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)