add_includedirs("../include")
set_languages("c++20")
add_cxxflags("-fconcepts")
add_links("pthread", "rt")
add_cxxflags("-g", "-Wall", "-Werror")
-- add_defines("SF_ASYNC_LOG")
add_defines("SF_DEBUG")
add_includedirs("../3rd/cocpp/include")

target("test")
set_kind("binary")
add_files("test.cpp")

