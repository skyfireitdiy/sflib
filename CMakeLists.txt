cmake_minimum_required(VERSION 3.7)

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
        libsf_meta/include
        libsf_meta_object/include
        libsf_type_list/include
        libsf_msg_bus/include
        libsf_chain/include
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
        libsf_meta/include/*.h
        libsf_meta_object/include/*.h
        libsf_type_list/include/*.h
        libsf_msg_bus/include/*.h
        libsf_chain/include/*.h
        )

install(FILES ${headers} DESTINATION include)


include_directories(test)


IF (WIN32)
    add_executable(server_main test/server_main.cpp ${headers})
    add_executable(client_main test/client_main.cpp ${headers})
    add_executable(sf_meta_object_test test/sf_meta_object_test.cpp ${headers})
    add_executable(test_prij test/test_proj.cpp ${headers})
    add_executable(aop_test test/aop_test.cpp ${headers})
    add_executable(test_sf_debug test/test_sf_debug.cpp ${headers})
    add_executable(test_object test/test_object.cpp ${headers})
    add_executable(test_thread_pool test/test_thread_pool.cpp ${headers})
    add_executable(test_type_list test/test_type_list.cpp ${headers})
    add_executable(test_msg_bus_server test/test_msg_bus_server.cpp ${headers})
    add_executable(test_msg_bus_client test/test_msg_bus_client.cpp ${headers})
    add_executable(test_any test/test_any.cpp ${headers})
    add_executable(test_chain_call test/test_chain_call.cpp ${headers})


    target_link_libraries(server_main ws2_32)
    target_link_libraries(client_main ws2_32)
    target_link_libraries(test_msg_bus_server ws2_32)
    target_link_libraries(test_msg_bus_client ws2_32)
ELSE()
    add_executable(server_main test/server_main.cpp ${headers})
    add_executable(client_main test/client_main.cpp ${headers})
    add_executable(sf_meta_object_test test/sf_meta_object_test.cpp ${headers})
    add_executable(test_prij test/test_proj.cpp ${headers})
    add_executable(aop_test test/aop_test.cpp ${headers})
    add_executable(test_sf_debug test/test_sf_debug.cpp ${headers})
    add_executable(test_object test/test_object.cpp ${headers})
    add_executable(test_thread_pool test/test_thread_pool.cpp ${headers})
    add_executable(test_type_list test/test_type_list.cpp ${headers})
    add_executable(test_msg_bus_server test/test_msg_bus_server.cpp ${headers})
    add_executable(test_msg_bus_client test/test_msg_bus_client.cpp ${headers})
    add_executable(test_any test/test_any.cpp ${headers})
    add_executable(test_chain_call test/test_chain_call.cpp ${headers})

    target_link_libraries(test_prij pthread)
    target_link_libraries(server_main pthread)
    target_link_libraries(client_main pthread)
    target_link_libraries(test_msg_bus_client pthread)
    target_link_libraries(test_msg_bus_server pthread)
    target_link_libraries(test_object pthread)
    target_link_libraries(test_thread_pool pthread)
    target_link_libraries(test_chain_call pthread)
ENDIF ()


