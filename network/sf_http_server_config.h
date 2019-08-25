
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_http_server_config.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#include "tools/sf_json.hpp"

namespace skyfire {

/**
 * @brief  http服务器配置项
 */
struct sf_http_server_config final {
    std::string host = "0.0.0.0";                     // 主机ip地址
    unsigned short port = 80;                         // http端口
    std::string tmp_file_path = ".";                  // 临时文件路径
    int session_timeout = 30 * 60;                    // session 过期时间
    int max_cache_count = default_cache_max_count;    // 最大cache 条数
    int max_cache_file_size =
        default_cache_max_file_size;    // 最大cache
                                        // 文件大小，超过此大小的文件不缓存
    bool debug = false;    // debug 模式不会使用缓存
};

SF_JSONIFY(sf_http_server_config, host, port, tmp_file_path, session_timeout,
           max_cache_count, max_cache_file_size, debug)
}    // namespace skyfire
