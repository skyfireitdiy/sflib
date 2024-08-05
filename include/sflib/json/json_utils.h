
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace skyfire
{
enum class json_type
{
    null = 0,
    boolean = 1,
    number = 2,
    string = 3,
    array = 4,
    object = 5,
};
struct json_value final
{
    json_type type;                                       // 类型
    std::string value;                                    // 值
    std::vector<std::shared_ptr<json_value>> array_value; // 数组类型
    std::unordered_map<std::string, std::shared_ptr<json_value>>
        object_value; // 对象类型
};
} // namespace skyfire