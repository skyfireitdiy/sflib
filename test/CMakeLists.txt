cmake_minimum_required(VERSION 3.7)

project(sflib_example)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
include_directories(
    ${OPENSSL_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
)
link_directories(
    ${ZLIB_LIBRARUES}
)

include_directories(.. .)

file(GLOB headers
        compiler/*
        core/*
        network/*
        tools/*
        )

set(CMAKE_CXX_FLAGS "-g")

add_executable(test_sf_chan sf_chan/test_sf_chan.cpp ${headers})
add_executable(test_sf_string sf_string/test_sf_string.cpp ${headers})



IF (WIN32)

ELSE()
    target_link_libraries(test_sf_chan pthread)
    target_link_libraries(test_sf_string pthread)
    
ENDIF ()


