#pragma once

#include "sf_object_class_meta_helper.h"
#include "sf_object.hpp"

namespace skyfire
{
    sf_object_class_meta_helper::sf_object_class_meta_helper(std::function<void()> func) {
        func();
    }
}