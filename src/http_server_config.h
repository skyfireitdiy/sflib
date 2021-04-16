
#pragma once
#include "json.hpp"
#include <thread>
namespace skyfire
{
struct http_server_config final
{
    std::string    host                = "0.0.0.0";                   // 主机ip地址
    unsigned short port                = 80;                          // http端口
    std::string    tmp_file_path       = ".";                         // 临时文件路径
    size_t         session_timeout     = 30 * 60;                     // session 过期时间
    size_t         max_cache_count     = default_cache_max_count;     // 最大cache 条数
    size_t         max_cache_file_size = default_cache_max_file_size; // 最大cache
        // 文件大小，超过此大小的文件不缓存
    bool   debug        = false;                                    // debug 模式不会使用缓存
    size_t thread_count = std::thread::hardware_concurrency() * 16; // 线程数量
};
SF_JSONIFY(http_server_config, host, port, tmp_file_path, session_timeout,
           max_cache_count, max_cache_file_size, debug, thread_count)
} // namespace skyfire
