set_languages("c++20")
add_cxxflags("-fconcepts")
add_cxxflags("-g", "-Wall", "-Werror")

kind = "shared"

target("argv")
set_kind(kind)
add_files("src/sflib/argv/*.cpp")
add_headerfiles("include/(sflib/argv/*.h)")
add_includedirs("include/")

target("cache")
set_kind(kind)
add_files("src/sflib/cache/*.cpp")
add_headerfiles("include/(sflib/cache/*.h)")
add_includedirs("include/")

target("color_string")
set_kind(kind)
add_files("src/sflib/color_string/*.cpp")
add_headerfiles("include/(sflib/color_string/*.h)")
add_includedirs("include/")

target("compiler")
set_kind(kind)
add_files("src/sflib/compiler/*.cpp")
add_headerfiles("include/(sflib/compiler/*.h)")
add_includedirs("include/")

target("core")
set_kind(kind)
add_files("src/sflib/core/*.cpp")
add_headerfiles("include/(sflib/core/*.h)")
add_includedirs("include/")

target("http")
set_kind(kind)
add_files("src/sflib/http/*.cpp")
add_headerfiles("include/(sflib/http/*.h)")
add_includedirs("include/")

target("io")
set_kind(kind)
add_files("src/sflib/io/*.cpp")
add_headerfiles("include/(sflib/io/*.h)")
add_includedirs("include/")

target("json")
set_kind(kind)
add_files("src/sflib/json/*.cpp")
add_headerfiles("include/(sflib/json/*.h)")
add_includedirs("include/")

target("msg_bus")
set_kind(kind)
add_files("src/sflib/msg_bus/*.cpp")
add_headerfiles("include/(sflib/msg_bus/*.h)")
add_includedirs("include/")

target("object")
set_kind(kind)
add_files("src/sflib/object/*.cpp")
add_headerfiles("include/(sflib/object/*.h)")
add_includedirs("include/")

target("rpc")
set_kind(kind)
add_files("src/sflib/rpc/*.cpp")
add_headerfiles("include/(sflib/rpc/*.h)")
add_includedirs("include/")

target("table")
set_kind(kind)
add_files("src/sflib/table/*.cpp")
add_headerfiles("include/(sflib/table/*.h)")
add_includedirs("include/")

target("tcp")
set_kind(kind)
add_files("src/sflib/tcp/*.cpp")
add_headerfiles("include/(sflib/tcp/*.h)")
add_includedirs("include/")

target("test")
set_kind(kind)
add_files("src/sflib/test/*.cpp")
add_headerfiles("include/(sflib/test/*.h)")
add_includedirs("include/")

target("thread_pool")
set_kind(kind)
add_files("src/sflib/thread_pool/*.cpp")
add_headerfiles("include/(sflib/thread_pool/*.h)")
add_includedirs("include/")

target("tools")
set_kind(kind)
add_files("src/sflib/tools/*.cpp")
add_headerfiles("include/(sflib/tools/*.h)")
add_includedirs("include/")

target("sf")
set_kind(kind)
add_deps("thread_pool", "table", "tools", "core", "http", "io", "msg_bus",
         "object", "rpc", "tcp", "test", "compiler", "color_string", "cache",
         "json", "argv")

target("test-case")
set_kind("binary")
add_includedirs("include/")
add_files("test/test.cpp")
add_deps("sf")
add_ldflags("-lpthread", "-lrt", "-lcrypto", "-lz")
