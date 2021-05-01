
#pragma once
#include "json.h"
#include <bits/stdc++.h>
namespace skyfire
{
struct object_factory_config_item_t
{
    std::string id;                // 对象id
    std::any    object;            // 对象实例
    bool        singleton = false; // 是否是单例
    json        data;              // json 数据
};
} // namespace skyfire