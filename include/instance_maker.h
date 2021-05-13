#pragma once
#include <bits/stdc++.h>

namespace skyfire
{

template <typename T, typename Base = empty_class>
struct instance_maker : public Base
{
private:
    instance_maker() = default;

public:
    template <typename... Args>
    static std::shared_ptr<T> make_instance(Args&&... args)
    {
        return std::shared_ptr<T>(new T(std::forward<Args>(args)...));
    }
    using Base::Base;
    friend T;
};
} // namespace skyfire
