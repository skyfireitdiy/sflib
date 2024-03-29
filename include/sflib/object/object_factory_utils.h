
#pragma once
#include "sflib/json/json.h"

#include <any>

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