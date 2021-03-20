
#pragma once
#include "empty_class.h"
#include "stdc++.h"
namespace skyfire
{
template <typename ThisClass, typename BaseClass = empty_class>
class single_instance : public BaseClass
{
public:
    template <typename... Args>
    static std::shared_ptr<ThisClass> instance(Args&&... args);

private:
    single_instance() = default;
    friend ThisClass;
};
} // namespace skyfire
#define SF_SINGLE_TON(ClassName)                            \
    ClassName(const ClassName&) = delete;                   \
    ClassName(ClassName&&)      = delete;                   \
    ClassName&        operator=(const ClassName&) = delete; \
    static ClassName* instance()                            \
    {                                                       \
        static ClassName instance__;                        \
        return &instance__;                                 \
    }
