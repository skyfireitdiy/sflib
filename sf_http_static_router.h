
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_http_static_router.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#include <memory>
#include <string>
#include "sf_http_router.hpp"
#include "sf_define.h"
#include "sf_http_utils.hpp"
#include "sf_logger.hpp"


namespace skyfire {
    using namespace std::literals;

    /**
     * 创建静态http路由
     * @param path 本地路径
     * @param methods 请求方式
     * @param charset 编码
     * @param deflate 是否启用压缩
     * @param max_file_size 最大文件大小
     * @param priority 优先级
     * @return 本地静态http路由
     */
    inline std::shared_ptr<sf_http_router>
    make_static_router(std::string path, const std::vector<std::string> &methods = {{"*"s}},
                       std::string charset = "utf-8",
                       bool deflate = true,
                       unsigned long long max_file_size = default_http_max_file_size,
                       int priority = default_http_static_priority

    );
}