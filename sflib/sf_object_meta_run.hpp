
/**
* @version 1.0.0
* @author skyfire
* @file sf_object_meta_run.hpp

* 
*/

#pragma once

#include "sf_object_meta_run.h"

namespace skyfire
{
inline object_meta_run::object_meta_run(const std::function<void()> f)
{
    f();
}
} // namespace skyfire