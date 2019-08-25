#pragma once

#include "core/sf_stdc++.h"

namespace skyfire {
/**
 * @brief json数据类型
 */
enum class sf_json_type {
    null = 0,
    boolean = 1,
    number = 2,
    string = 3,
    array = 4,
    object = 5,
};

/**
 * json 值结构
 */
struct sf_json_value final {
    sf_json_type type;                                          // 类型
    std::string value;                                          // 值
    std::vector<std::shared_ptr<sf_json_value>> array_value;    // 数组类型
    std::unordered_map<std::string, std::shared_ptr<sf_json_value>>
        object_value;    // 对象类型
};
}    // namespace skyfire