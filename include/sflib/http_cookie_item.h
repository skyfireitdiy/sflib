#pragma once
#include "stdc++.h"

namespace skyfire
{

enum class cookie_life_type
{
    session    = 0, // session生存期
    time_point = 1  // 时间点生存期
};

struct http_cookie_item_t final
{
    std::string                           key;                                           // 键
    std::string                           value;                                         // 值
    cookie_life_type                      life_type  = cookie_life_type ::session;       // 生存期类型
    std::chrono::system_clock::time_point time_point = std::chrono::system_clock::now(); // 生存期
    std::string                           path       = "/";                              // 路径
    bool                                  secure     = false;                            // 安全性
    bool                                  http_only  = true;                             // http only
};
}