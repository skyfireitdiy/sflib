cmake_minimum_required(VERSION 3.7)

project(sflib)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

include_directories(
        ${OPENSSL_INCLUDE_DIRS}
        ${ZLIB_INCLUDE_DIRS}
        .
)

link_directories(
        ${ZLIB_LIBRARUES}
)

file(GLOB headers
        *.h
        *.hpp
        )

install(FILES ${headers} DESTINATION include)


include_directories(test)

add_executable(test_rpc_server test/test_rpc/test_rpc_server.cpp ${headers})
add_executable(test_rpc_client test/test_rpc/test_rpc_client.cpp ${headers})
add_executable(test_aop test/test_aop/test_aop.cpp ${headers})
add_executable(test_sf_logger test/test_sf_logger/test_sf_logger.cpp ${headers})
add_executable(test_object test/test_object/test_object.cpp ${headers})
add_executable(test_thread_pool test/test_thread_pool/test_thread_pool.cpp ${headers})
add_executable(test_type_list test/test_type_list/test_type_list.cpp ${headers})
add_executable(test_msg_bus_server test/test_msg_bus/test_msg_bus_server.cpp ${headers})
add_executable(test_msg_bus_client test/test_msg_bus/test_msg_bus_client.cpp ${headers})
add_executable(test_tcp_nat_traversal_server test/test_tcp_nat_traversal/test_tcp_nat_traversal_server.cpp ${headers})
add_executable(test_tcp_nat_traversal_client test/test_tcp_nat_traversal/test_tcp_nat_traversal_client.cpp ${headers})
add_executable(test_event_waiter test/test_event_waiter/test_event_waiter.cpp ${headers})
add_executable(test_httpserver test/test_http_server/test_http_server.cpp ${headers})
add_executable(test_tcpserver test/test_tcp_server/test_tcp_server.cpp ${headers})
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
    target_link_libraries(test_httpserver pthread ssl crypto z)

ENDIF ()


