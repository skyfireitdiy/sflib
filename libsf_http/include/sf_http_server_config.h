
/**
* @version 1.0.0
* @author skyfire
* @email skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_http_server_config.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#include <string>
#include "sf_serialize_binary.hpp"

namespace skyfire {

    /**
     * @brief  http服务器配置项
     */
    struct sf_http_server_config {
        std::string host;               // 主机ip地址
        unsigned short port;          // http端口
        size_t thread_count;            // 线程数量
        int request_timeout;            // 连接超时
        std::string tmp_file_path;      // 临时文件路径
    };
}