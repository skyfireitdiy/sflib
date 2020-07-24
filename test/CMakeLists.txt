cmake_minimum_required( VERSION 3.7)

SET (CMAKE_C_COMPILER             "/usr/bin/clang")
SET (CMAKE_C_FLAGS                "-Wall -std=c99")
SET (CMAKE_C_FLAGS_DEBUG          "-g")
SET (CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
SET (CMAKE_C_FLAGS_RELEASE        "-O4 -DNDEBUG")
SET (CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

SET (CMAKE_CXX_COMPILER             "/usr/bin/clang++")
SET (CMAKE_CXX_FLAGS                "-Wall")
SET (CMAKE_CXX_FLAGS_DEBUG          "-g")
SET (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

SET (CMAKE_AR      "/usr/bin/llvm-ar")
SET (CMAKE_LINKER  "/usr/bin/llvm-ld")
SET (CMAKE_NM      "/usr/bin/llvm-nm")
SET (CMAKE_OBJDUMP "/usr/bin/llvm-objdump")
SET (CMAKE_RANLIB  "/usr/bin/llvm-ranlib")

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
        *
        sflib/*
        )

set(CMAKE_CXX_FLAGS "-g")

add_executable(test_sf_chan sf_chan/test_sf_chan.cpp ${headers})
add_executable(test_sf_string sf_string/test_sf_string.cpp ${headers})
add_executable(test_sf_argv sf_argv/test_sf_argv.cpp ${headers})
add_executable(test_sf_cache sf_cache/test_sf_cache.cpp ${headers})
add_executable(test_sf_event_waiter sf_event_waiter/test_sf_event_waiter.cpp ${headers})
add_executable(test_sf_finally sf_finally/test_sf_finally.cpp ${headers})



IF (WIN32)

ELSE()
    target_link_libraries(test_sf_chan pthread)
    target_link_libraries(test_sf_string pthread)
    target_link_libraries(test_sf_argv pthread)
    target_link_libraries(test_sf_cache pthread)
    target_link_libraries(test_sf_event_waiter pthread)
    target_link_libraries(test_sf_finally pthread)
    
    
    
ENDIF ()


