cmake_minimum_required(VERSION 3.7)

project(sflib)

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



file(GLOB headers
        compiler/*.h
        compiler/*.hpp
        core/*.h
        core/*.hpp
        network/*.h
        network/*.hpp
        tools/*.h
        tools/*.hpp
        )


include_directories(example)

add_executable(test_rpc_server example/test_rpc/test_rpc_server.cpp ${headers})
add_executable(test_rpc_client example/test_rpc/test_rpc_client.cpp ${headers})
add_executable(test_aop example/test_aop/test_aop.cpp ${headers})
add_executable(test_sf_logger example/test_sf_logger/test_sf_logger.cpp ${headers})
add_executable(test_object example/test_object/test_object.cpp ${headers})
add_executable(test_thread_pool example/test_thread_pool/test_thread_pool.cpp ${headers})
add_executable(test_type_list example/test_type_list/test_type_list.cpp ${headers})
add_executable(test_msg_bus_server example/test_msg_bus/test_msg_bus_server.cpp ${headers})
add_executable(test_msg_bus_client example/test_msg_bus/test_msg_bus_client.cpp ${headers})
add_executable(test_tcp_nat_traversal_server example/test_tcp_nat_traversal/test_tcp_nat_traversal_server.cpp ${headers})
add_executable(test_tcp_nat_traversal_client example/test_tcp_nat_traversal/test_tcp_nat_traversal_client.cpp ${headers})
add_executable(test_event_waiter example/test_event_waiter/test_event_waiter.cpp ${headers})
add_executable(test_httpserver example/test_http_server/test_http_server.cpp ${headers})
add_executable(test_tcpserver example/test_tcp_server/test_tcp_server.cpp ${headers})
add_executable(test_finally example/test_finally/test_finally.cpp ${headers})
add_executable(test_grammer example/test_grammer/test_grammer.cpp ${headers})
add_executable(test_json example/test_json/test_json.cpp ${headers})
add_executable(test_factory example/test_factory/test_factory.cpp ${headers})
add_executable(test_argv example/test_argv/test_argv.cpp ${headers})
add_executable(test_chan example/test_chan/test_chan.cpp ${headers})




IF (WIN32)
    target_link_libraries(test_rpc_server ws2_32)
    target_link_libraries(test_rpc_client ws2_32)
    target_link_libraries(test_msg_bus_server ws2_32)
    target_link_libraries(test_msg_bus_client ws2_32)
    target_link_libraries(test_tcp_nat_traversal_server ws2_32)
    target_link_libraries(test_tcp_nat_traversal_client ws2_32)
    target_link_libraries(test_tcpserver ws2_32)
    target_link_libraries(test_httpserver PRIVATE ws2_32 OpenSSL::SSL OpenSSL::Crypto ZLIB::ZLIB)
ELSE()
    target_link_libraries(test_rpc_server pthread)
    target_link_libraries(test_rpc_client pthread)
    target_link_libraries(test_msg_bus_client pthread)
    target_link_libraries(test_msg_bus_server pthread)
    target_link_libraries(test_object pthread)
    target_link_libraries(test_thread_pool pthread)
    target_link_libraries(test_tcp_nat_traversal_server pthread)
    target_link_libraries(test_tcp_nat_traversal_client pthread)
    target_link_libraries(test_tcpserver pthread)
    target_link_libraries(test_sf_logger pthread)
    target_link_libraries(test_event_waiter pthread)
    target_link_libraries(test_httpserver pthread ssl crypto z stdc++fs)
    target_link_libraries(test_aop pthread)
    target_link_libraries(test_json pthread)
    target_link_libraries(test_factory pthread)
    target_link_libraries(test_argv pthread)
    target_link_libraries(test_chan pthread)

ENDIF ()


