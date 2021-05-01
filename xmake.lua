
add_includedirs("include", ".")
set_languages("c++20")
add_cxxflags("-fconcepts")
add_links("pthread", "rt")
add_cxxflags("-g", "-Wall", "-Werror")

-- add_defines("SF_ASYNC_LOG") -- 日志使用协程异步输出
-- add_defines("SF_BIG_JSON")  -- 支持大结构体转json，支持1000个成员对象，默认仅支持32个成员
-- add_defines("SF_DEBUG")  -- 打印调试信息

target("sflib_static")
    set_kind("static")
    add_files("./src/*.cpp")

target("sflib_shared")
    set_kind("shared")
    add_files("./src/*.cpp")

target("test")
    set_kind("binary")
    add_files("./test/test.cpp")
    add_deps("sflib_static")
