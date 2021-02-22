
add_includedirs("../")
set_languages("c++20")
add_links("pthread", "rt")
add_cxxflags("-g", "-Wall", "-Werror")
target("test")
    set_kind("binary")
    add_files("test.cpp")
