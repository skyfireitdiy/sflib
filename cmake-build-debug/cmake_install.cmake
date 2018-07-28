# Install script for directory: /home/skyfire/CLionProjects/sflib

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/home/skyfire/CLionProjects/sflib/libsf_logger/include/sf_logger.h"
    "/home/skyfire/CLionProjects/sflib/libsf_empty_class/include/sf_empty_class.h"
    "/home/skyfire/CLionProjects/sflib/libsf_finally/include/sf_finally.h"
    "/home/skyfire/CLionProjects/sflib/libsf_nocopy/include/sf_nocopy.h"
    "/home/skyfire/CLionProjects/sflib/libsf_object/include/sf_event_waiter.h"
    "/home/skyfire/CLionProjects/sflib/libsf_object/include/sf_eventloop.h"
    "/home/skyfire/CLionProjects/sflib/libsf_object/include/sf_msg_queue.h"
    "/home/skyfire/CLionProjects/sflib/libsf_object/include/sf_object.h"
    "/home/skyfire/CLionProjects/sflib/libsf_object/include/sf_timer.h"
    "/home/skyfire/CLionProjects/sflib/libsf_range/include/sf_range.h"
    "/home/skyfire/CLionProjects/sflib/libsf_rpc/include/sf_rpcclient.h"
    "/home/skyfire/CLionProjects/sflib/libsf_rpc/include/sf_rpcserver.h"
    "/home/skyfire/CLionProjects/sflib/libsf_serialize/include/sf_serialize_binary.h"
    "/home/skyfire/CLionProjects/sflib/libsf_single_instance/include/sf_single_instance.h"
    "/home/skyfire/CLionProjects/sflib/libsf_net/include/sf_netutils.h"
    "/home/skyfire/CLionProjects/sflib/libsf_net/include/sf_tcpclient.h"
    "/home/skyfire/CLionProjects/sflib/libsf_net/include/sf_tcpclient_linux.h"
    "/home/skyfire/CLionProjects/sflib/libsf_net/include/sf_tcpclient_win.h"
    "/home/skyfire/CLionProjects/sflib/libsf_net/include/sf_tcpserver.h"
    "/home/skyfire/CLionProjects/sflib/libsf_net/include/sf_tcpserver_linux.h"
    "/home/skyfire/CLionProjects/sflib/libsf_net/include/sf_tcpserver_win.h"
    "/home/skyfire/CLionProjects/sflib/libsf_net/include/sf_tcputils.h"
    "/home/skyfire/CLionProjects/sflib/libsf_thread_pool/include/sf_thread_pool.h"
    "/home/skyfire/CLionProjects/sflib/libsf_tri_type/include/sf_tri_type.h"
    "/home/skyfire/CLionProjects/sflib/libsf_watch_time/include/sf_watch_time.h"
    "/home/skyfire/CLionProjects/sflib/libsf_type/include/sf_type.h"
    "/home/skyfire/CLionProjects/sflib/libsf_meta/include/sf_meta.h"
    "/home/skyfire/CLionProjects/sflib/libsf_meta_object/include/sf_meta_object.h"
    "/home/skyfire/CLionProjects/sflib/libsf_type_list/include/sf_type_list.h"
    "/home/skyfire/CLionProjects/sflib/libsf_msg_bus/include/sf_msg_bus_client.h"
    "/home/skyfire/CLionProjects/sflib/libsf_msg_bus/include/sf_msg_bus_server.h"
    "/home/skyfire/CLionProjects/sflib/libsf_msg_bus/include/sf_msg_bus_utils.h"
    "/home/skyfire/CLionProjects/sflib/libsf_chain/include/sf_chain.h"
    "/home/skyfire/CLionProjects/sflib/libsf_tcp_nat_traversal/include/sf_tcp_nat_traversal_client.h"
    "/home/skyfire/CLionProjects/sflib/libsf_tcp_nat_traversal/include/sf_tcp_nat_traversal_server.h"
    "/home/skyfire/CLionProjects/sflib/libsf_tcp_nat_traversal/include/sf_tcp_nat_traversal_utils.h"
    "/home/skyfire/CLionProjects/sflib/libsf_tl/include/sf_bimap.h"
    "/home/skyfire/CLionProjects/sflib/libsf_define/include/sf_define.h"
    "/home/skyfire/CLionProjects/sflib/libsf_random/include/sf_random.h"
    "/home/skyfire/CLionProjects/sflib/libsf_http/include/sf_content_type.h"
    "/home/skyfire/CLionProjects/sflib/libsf_http/include/sf_http_base_server.h"
    "/home/skyfire/CLionProjects/sflib/libsf_http/include/sf_http_header.h"
    "/home/skyfire/CLionProjects/sflib/libsf_http/include/sf_http_request.h"
    "/home/skyfire/CLionProjects/sflib/libsf_http/include/sf_http_request_line.h"
    "/home/skyfire/CLionProjects/sflib/libsf_http/include/sf_http_response.h"
    "/home/skyfire/CLionProjects/sflib/libsf_http/include/sf_http_router.h"
    "/home/skyfire/CLionProjects/sflib/libsf_http/include/sf_http_server.h"
    "/home/skyfire/CLionProjects/sflib/libsf_http/include/sf_http_server_config.h"
    "/home/skyfire/CLionProjects/sflib/libsf_http/include/sf_http_static_router.h"
    "/home/skyfire/CLionProjects/sflib/libsf_http/include/sf_http_utils.h"
    "/home/skyfire/CLionProjects/sflib/libsf_http/include/sf_websocket_router.h"
    "/home/skyfire/CLionProjects/sflib/libsf_http/include/sf_websocket_utils.h"
    "/home/skyfire/CLionProjects/sflib/libsf_utils/include/sf_utils.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/skyfire/CLionProjects/sflib/cmake-build-debug/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
