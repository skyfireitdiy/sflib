
#pragma once
#include "object_meta_run.h"
namespace skyfire
{
inline object_meta_run::object_meta_run(const std::function<void()> f)
{
    f();
}
} // namespace skyfire