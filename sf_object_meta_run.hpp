#pragma once

#include "sf_object_meta_run.h"

#include <iostream>

using namespace std;

namespace skyfire
{
    inline sf_object_meta_run::sf_object_meta_run(std::function<void()> f)
    {
        f();
    }
}