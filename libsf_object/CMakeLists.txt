cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(sf_object VERSION 0.1.0 LANGUAGES CXX)

include_directories("${PROJECT_SOURCE_DIR}/../libsf_empty_class/include")
include_directories("${PROJECT_SOURCE_DIR}/../libsf_single_instance/include")
include_directories("${PROJECT_SOURCE_DIR}/../libsf_nocopy/include")
include_directories("${PROJECT_SOURCE_DIR}/include")

add_library(sf_object SHARED src/sf_object.cpp include/sf_object.h src/sf_msg_queue.cpp include/sf_msg_queue.h src/sf_eventloop.cpp include/sf_eventloop.h src/sf_timer.cpp include/sf_timer.h)

add_library(sf_object_static STATIC src/sf_object.cpp include/sf_object.h src/sf_msg_queue.cpp include/sf_msg_queue.h src/sf_eventloop.cpp include/sf_eventloop.h src/sf_timer.cpp include/sf_timer.h)

install(FILES "${PROJECT_SOURCE_DIR}/include/sf_object.h" DESTINATION include)

install(TARGETS sf_object sf_object_static
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )
