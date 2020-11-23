
add_includedirs("../")

set_languages("c++20")
add_links("pthread")

add_cxxflags("-g")

target("sf_argv")
    set_kind("binary")
    add_files("sf_argv/*.cpp")
target("sf_cache")
    set_kind("binary")
    add_files("sf_cache/*.cpp")
target("sf_chan")
    set_kind("binary")
    add_files("sf_chan/*.cpp")
target("sf_event_waiter")
    set_kind("binary")
    add_files("sf_event_waiter/*.cpp")
target("sf_finally")
    set_kind("binary")
    add_files("sf_finally/*.cpp")
target("sf_multi_value")
    set_kind("binary")
    add_files("sf_multi_value/*.cpp")
target("sf_string")
    set_kind("binary")
    add_files("sf_string/*.cpp")
target("sf_json")
    set_kind("binary")
    add_files("sf_json/*.cpp")
target("sf_data_buffer")
    set_kind("binary")
    add_files("sf_data_buffer/*.cpp")
target("sf_http")
    set_kind("binary")
    add_files("sf_http/*.cpp")