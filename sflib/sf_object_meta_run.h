
#pragma once
#include <functional>
namespace skyfire
{
class object_meta_run final
{
public:
    explicit object_meta_run(const std::function<void()> f);
};
} // namespace skyfire