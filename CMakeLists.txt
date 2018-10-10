cmake_minimum_required(VERSION 3.7)

project(sflib)

if(WIN32)

else()
#    # 找到G++ 7/8的位置
#    execute_process(COMMAND which g++-7
#            OUTPUT_VARIABLE GCC7_PATH
#            )
#    execute_process(COMMAND which g++-8
#            OUTPUT_VARIABLE GCC8_PATH
#            )
#
#    # 去除which命令最后的换行
#    string(STRIP "${GCC7_PATH}" GCC7_PATH)
#    string(STRIP "${GCC8_PATH}" GCC8_PATH)
#
#    # 判断有效变量
#    string(COMPARE NOTEQUAL "${GCC7_PATH}" "" GCC7_OK)
#    string(COMPARE NOTEQUAL "${GCC8_PATH}" "" GCC8_OK)
#
#    # 设置编译器
#    if(${GCC7_OK})
#        message(STATUS "Find g++-7 ---> ${GCC7_PATH}")
#        message(STATUS "Set CMAKE_CXX_COMPILER=${GCC7_PATH}")
#        set(CMAKE_CXX_COMPILER ${GCC7_PATH})
#    elseif(${GCC8_OK})
#        message(STATUS "Find g++-8 ---> ${GCC8_PATH}")
#        message(STATUS "Set CMAKE_CXX_COMPILER=${GCC8_PATH}")
#        set(CMAKE_CXX_COMPILER ${GCC8_PATH})
#    else()
#        message(STATUS "Can't find g++-7 and g++-8, use default g++ !")
#    endif()
endif()


set(CMAKE_CXX_STANDARD 17)


find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)



include_directories(
        ${OPENSSL_INCLUDE_DIRS}
        ${ZLIB_INCLUDE_DIRS}
        libsf_any/include
        libsf_logger/include
        libsf_empty_class/include
        libsf_finally/include
        libsf_nocopy/include
        libsf_object/include
        libsf_range/include
        libsf_rpc/include
        libsf_serialize/include
        libsf_single_instance/include
        libsf_net/include
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
        libsf_random/include
        libsf_http/include
        libsf_utils/include

)

link_directories(
        
        ${ZLIB_LIBRARUES}
)



file(GLOB headers
        libsf_any/include/*.h
        libsf_logger/include/*.h
        libsf_empty_class/include/*.h
        libsf_finally/include/*.h
        libsf_nocopy/include/*.h
        libsf_object/include/*.h
        libsf_range/include/*.h
        libsf_rpc/include/*.h
        libsf_serialize/include/*.h
        libsf_single_instance/include/*.h
        libsf_net/include/*.h
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
        libsf_random/include/*.h
        libsf_http/include/*.h
        libsf_utils/include/*.h
        libsf_component_system/*.h

        libsf_any/include/*.hpp
        libsf_logger/include/*.hpp
        libsf_empty_class/include/*.hpp
        libsf_finally/include/*.hpp
        libsf_nocopy/include/*.hpp
        libsf_object/include/*.hpp
        libsf_range/include/*.hpp
        libsf_rpc/include/*.hpp
        libsf_serialize/include/*.hpp
        libsf_single_instance/include/*.hpp
        libsf_net/include/*.hpp
        libsf_thread_pool/include/*.hpp
        libsf_tri_type/include/*.hpp
        libsf_watch_time/include/*.hpp
        libsf_type/include/*.hpp
        libsf_meta/include/*.hpp
        libsf_meta_object/include/*.hpp
        libsf_type_list/include/*.hpp
        libsf_msg_bus/include/*.hpp
        libsf_chain/include/*.hpp
        libsf_tcp_nat_traversal/include/*.hpp
        libsf_tl/include/*.hpp
        libsf_define/include/*.hpp
        libsf_random/include/*.hpp
        libsf_http/include/*.hpp
        libsf_utils/include/*.hpp
        libsf_component_system/*.hpp
        )

install(FILES ${headers} DESTINATION include)


include_directories(test)

add_executable(test_rpc_server test/test_rpc/test_rpc_server.cpp ${headers})
add_executable(test_rpc_client test/test_rpc/test_rpc_client.cpp ${headers})
add_executable(sf_meta_object_test test/test_meta_object/test_meta_object.cpp ${headers})
add_executable(test_meta test/test_meta/test_meta.cpp ${headers})
add_executable(test_aop test/test_aop/test_aop.cpp ${headers})
add_executable(test_sf_logger test/test_sf_logger/test_sf_logger.cpp ${headers})
add_executable(test_object test/test_object/test_object.cpp ${headers})
add_executable(test_thread_pool test/test_thread_pool/test_thread_pool.cpp ${headers})
add_executable(test_type_list test/test_type_list/test_type_list.cpp ${headers})
add_executable(test_msg_bus_server test/test_msg_bus/test_msg_bus_server.cpp ${headers})
add_executable(test_msg_bus_client test/test_msg_bus/test_msg_bus_client.cpp ${headers})
add_executable(test_chain_call test/test_chain_call/test_chain_call.cpp ${headers})
add_executable(test_timer test/test_timer/test_timer.cpp ${headers})
add_executable(test_tcp_nat_traversal_server test/test_tcp_nat_traversal/test_tcp_nat_traversal_server.cpp ${headers})
add_executable(test_tcp_nat_traversal_client test/test_tcp_nat_traversal/test_tcp_nat_traversal_client.cpp ${headers})
add_executable(test_event_waiter test/test_event_waiter/test_event_waiter.cpp ${headers})
add_executable(test_httpserver test/test_http_server/test_http_server.cpp ${headers})
add_executable(test_encrypt test/test_encrypt/test_encrypt.cpp ${headers})
add_executable(test_tcpserver test/test_tcpserver/test_tcpserver.cpp ${headers})
add_executable(test_finally test/test_finally/test_finally.cpp ${headers})




IF (WIN32)
    target_link_libraries(test_rpc_server ws2_32)
    target_link_libraries(test_rpc_client ws2_32)
    target_link_libraries(test_msg_bus_server ws2_32)
    target_link_libraries(test_msg_bus_client ws2_32)
    target_link_libraries(test_tcp_nat_traversal_server ws2_32)
    target_link_libraries(test_tcp_nat_traversal_client ws2_32)
    target_link_libraries(test_tcpserver ws2_32)
    target_link_libraries(test_httpserver ws2_32 ${OPENSSL_LIBRARIES} z)
	target_link_libraries(test_encrypt PRIVATE ${OPENSSL_LIBRARIES} z)
ELSE()
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
    target_link_libraries(test_tcpserver pthread)
    target_link_libraries(test_sf_logger pthread)
    target_link_libraries(test_event_waiter pthread)
    target_link_libraries(test_httpserver pthread ssl crypto z)
	target_link_libraries(test_encrypt PRIVATE ssl crypto z pthread)
ENDIF ()


