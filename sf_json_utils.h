#pragma once

#include "sf_stdc++.h"

namespace skyfire
{
    /**
     * @brief json数据类型
     */
    enum class sf_json_type
    {
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
    struct sf_json_value final
    {
        sf_json_type type;                                                          // 类型
        long double number_value;                                                   // 数值类型（bool类型）
        std::string string_value;                                                   // 字符串类型
        std::vector<std::shared_ptr<sf_json_value>> array_value;                    // 数组类型
        std::unordered_map<std::string, std::shared_ptr<sf_json_value>> object_value;         // 对象类型
    };
}