
#pragma once
#include "sflib/tools/empty_class.h"
#include <memory>

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

template <typename ThisClass, typename BaseClass>
template <typename... Args>
std::shared_ptr<ThisClass> single_instance<ThisClass, BaseClass>::instance(
    Args&&... args)
{
    static auto instance__ = std::shared_ptr<ThisClass>(
        new ThisClass(std::forward<Args>(args)...));
    return instance__;
}

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
