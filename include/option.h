#pragma once
#include <functional>
template <typename TargetType>
class option
{
public:
    using OptionFuncType = std::function<void(TargetType&)>;
    template <typename T>
    static auto make_option(T func)
    {
        return make_option_impl__(std::function(func));
    }

    template <typename... Args>
    static std::function<OptionFuncType(Args...)> make_option_impl__(std::function<void(TargetType&, Args...)> func)
    {
        return [func](Args... args) -> OptionFuncType {
            return [func, args...](TargetType& target) {
                func(target, Args(args)...);
            };
        };
    }
};
