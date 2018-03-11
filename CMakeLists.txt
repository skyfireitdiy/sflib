cmake_minimum_required(VERSION 3.9)

project(sflib)

set(CMAKE_CXX_STANDARD 17)

include_directories(
        libsf_any/include
        libsf_debug/include
        libsf_empty_class/include
        libsf_finally/include
        libsf_nocopy/include
        libsf_object/include
        libsf_range/include
        libsf_rpc/include
        libsf_serialize/include
        libsf_single_instance/include
        libsf_tcp/include
        libsf_thread_pool/include
        libsf_tri_type/include
        libsf_watch_time/include
        libsf_type/include
)

file(GLOB headers
        libsf_any/include/*.h
        libsf_debug/include/*.h
        libsf_empty_class/include/*.h
        libsf_finally/include/*.h
        libsf_nocopy/include/*.h
        libsf_object/include/*.h
        libsf_range/include/*.h
        libsf_rpc/include/*.h
        libsf_serialize/include/*.h
        libsf_single_instance/include/*.h
        libsf_tcp/include/*.h
        libsf_thread_pool/include/*.h
        libsf_tri_type/include/*.h
        libsf_watch_time/include/*.h
        libsf_type/include/*.h
        )

install(FILES ${headers} DESTINATION include)


include_directories(test)

add_executable(server_main test/server_main.cpp ${headers})
add_executable(client_main test/client_main.cpp ${headers})


target_link_libraries(server_main ws2_32)
target_link_libraries(client_main ws2_32)
