
/**
* @version 1.0.0
* @author skyfire
* @file sf_object_factory_utils.h

* 
*/

#pragma once

#include "sf_stdc++.h"
#include "sf_json.h"

namespace skyfire {
/**
 * @brief 对象工厂配置项
 *
 */
struct object_factory_config_item_t {
    std::string id; // 对象id
    std::any object; // 对象实例
    bool singleton = false; // 是否是单例
    json data; // json 数据
};
} // namespace skyfire