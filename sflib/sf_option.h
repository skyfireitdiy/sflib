#pragma once

#include <functional>

/**
 * @brief 选项类
 * 
 * @tparam TargetType 目标实体类型
 */

template <typename TargetType>
class option
{
public:
    using OptionFuncType = std::function<void(TargetType&)>;

    template <typename... Args>
    static std::function<OptionFuncType(Args...)> make_option(std::function<void(TargetType&, Args...)> func)
    {
        return [func](Args... args) -> OptionFuncType {
            return [func, args...](TargetType& target) {
                func(target, Args(args)...);
            };
        };
    }
};
