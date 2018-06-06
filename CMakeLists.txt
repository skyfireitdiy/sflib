cmake_minimum_required(VERSION 3.7)

project(sflib)

if(WIN32)

else()
    # 找到G++ 7/8的位置
    execute_process(COMMAND which g++-7
            OUTPUT_VARIABLE GCC7_PATH
            )
    execute_process(COMMAND which g++-8
            OUTPUT_VARIABLE GCC8_PATH
            )

    # 去除which命令最后的换行
    string(STRIP "${GCC7_PATH}" GCC7_PATH)
    string(STRIP "${GCC8_PATH}" GCC8_PATH)

    # 判断有效变量
    string(COMPARE NOTEQUAL "${GCC7_PATH}" "" GCC7_OK)
    string(COMPARE NOTEQUAL "${GCC8_PATH}" "" GCC8_OK)

    # 设置编译器
    if(${GCC7_OK})
        message(STATUS "Find g++-7 ---> ${GCC7_PATH}")
        message(STATUS "Set CMAKE_CXX_COMPILER=${GCC7_PATH}")
        set(CMAKE_CXX_COMPILER ${GCC7_PATH})
    elseif(${GCC8_OK})
        message(STATUS "Find g++-8 ---> ${GCC8_PATH}")
        message(STATUS "Set CMAKE_CXX_COMPILER=${GCC8_PATH}")
        set(CMAKE_CXX_COMPILER ${GCC8_PATH})
    else()
        message(FATAL_ERROR "Please update you complier!")
    endif()
endif()


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
        libsf_tcp_nat_traversal/include
        libsf_tl/include
        libsf_define/include
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
        libsf_tcp_nat_traversal/include/*.h
        libsf_tl/include/*.h
        libsf_define/include/*.h
        )

install(FILES ${headers} DESTINATION include)


include_directories(test)


IF (WIN32)
    add_executable(test_rpc_server test/test_rpc/test_rpc_server.cpp ${headers})
    add_executable(test_rpc_client test/test_rpc/test_rpc_client.cpp ${headers})
    add_executable(sf_meta_object_test test/test_meta_object/test_meta_object.cpp ${headers})
    add_executable(test_meta test/test_meta/test_meta.cpp ${headers})
    add_executable(test_aop test/test_aop/test_aop.cpp ${headers})
    add_executable(test_sf_debug test/test_sf_debug/test_sf_debug.cpp ${headers})
    add_executable(test_object test/test_object/test_object.cpp ${headers})
    add_executable(test_thread_pool test/test_thread_pool/test_thread_pool.cpp ${headers})
    add_executable(test_type_list test/test_type_list/test_type_list.cpp ${headers})
    add_executable(test_msg_bus_server test/test_msg_bus/test_msg_bus_server.cpp ${headers})
    add_executable(test_msg_bus_client test/test_msg_bus/test_msg_bus_client.cpp ${headers})
    add_executable(test_any test/test_any/test_any.cpp ${headers})
    add_executable(test_chain_call test/test_chain_call/test_chain_call.cpp ${headers})
    add_executable(test_timer test/test_timer/test_timer.cpp ${headers})
    add_executable(test_tcp_nat_traversal_server test/test_tcp_nat_traversal/test_tcp_nat_traversal_server.cpp ${headers})
    add_executable(test_tcp_nat_traversal_client test/test_tcp_nat_traversal/test_tcp_nat_traversal_client.cpp ${headers})



    target_link_libraries(test_rpc_server ws2_32)
    target_link_libraries(test_rpc_client ws2_32)
    target_link_libraries(test_msg_bus_server ws2_32)
    target_link_libraries(test_msg_bus_client ws2_32)
    target_link_libraries(test_tcp_nat_traversal_server ws2_32)
    target_link_libraries(test_tcp_nat_traversal_client ws2_32)


ELSE()
    add_executable(test_rpc_server test/test_rpc/test_rpc_server.cpp ${headers})
    add_executable(test_rpc_client test/test_rpc/test_rpc_client.cpp ${headers})
    add_executable(test_meta_object test/test_meta_object/test_meta_object.cpp ${headers})
    add_executable(test_meta test/test_meta/test_meta.cpp ${headers})
    add_executable(test_aop test/test_aop/test_aop.cpp ${headers})
    add_executable(test_sf_debug test/test_sf_debug/test_sf_debug.cpp ${headers})
    add_executable(test_object test/test_object/test_object.cpp ${headers})
    add_executable(test_thread_pool test/test_thread_pool/test_thread_pool.cpp ${headers})
    add_executable(test_type_list test/test_type_list/test_type_list.cpp ${headers})
    add_executable(test_msg_bus_server test/test_msg_bus/test_msg_bus_server.cpp ${headers})
    add_executable(test_msg_bus_client test/test_msg_bus/test_msg_bus_client.cpp ${headers})
    add_executable(test_any test/test_any/test_any.cpp ${headers})
    add_executable(test_chain_call test/test_chain_call/test_chain_call.cpp ${headers})
    add_executable(test_timer test/test_timer/test_timer.cpp ${headers})
    add_executable(test_tcp_nat_traversal_server test/test_tcp_nat_traversal/test_tcp_nat_traversal_server.cpp ${headers})
    add_executable(test_tcp_nat_traversal_client test/test_tcp_nat_traversal/test_tcp_nat_traversal_client.cpp ${headers})

    target_link_libraries(test_meta pthread)
    target_link_libraries(test_rpc_server pthread)
    target_link_libraries(test_rpc_client pthread)
    target_link_libraries(test_msg_bus_client pthread)
    target_link_libraries(test_msg_bus_server pthread)
    target_link_libraries(test_object pthread)
    target_link_libraries(test_thread_pool pthread)
    target_link_libraries(test_chain_call pthread)
    target_link_libraries(test_timer pthread)
    target_link_libraries(test_tcp_nat_traversal_server pthread)
    target_link_libraries(test_tcp_nat_traversal_client pthread)

ENDIF ()


