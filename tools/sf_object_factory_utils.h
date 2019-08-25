#pragma once

#include "core/sf_stdc++.h"
#include "tools/sf_json.h"

namespace skyfire {
/**
 * @brief 对象工厂配置项
 *
 */
struct sf_object_factory_config_item_t {
    std::string id;            // 对象id
    std::any object;           // 对象实例
    bool singleton = false;    // 是否是单例
    sf_json data;              // json 数据
};
}    // namespace skyfire