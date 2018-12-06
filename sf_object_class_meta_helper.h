#pragma once

#include "sf_object.h"
#include <functional>

namespace skyfire
{
    class sf_object_class_meta_helper
    {
    public:
        sf_object_class_meta_helper(std::function<void()> func);
    };
}