
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

#include "sf_json.hpp"

namespace skyfire {

    /**
     * @brief  http服务器配置项
     */
    struct sf_http_server_config final
    {
        std::string host    = "0.0.0.0";               // 主机ip地址
        unsigned short port = 80;          // http端口
        int request_timeout = 30;            // 连接超时
        std::string tmp_file_path = ".";      // 临时文件路径
    };

    SF_JSONIFY(sf_http_server_config, host, port, request_timeout, tmp_file_path)
}

