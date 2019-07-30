#pragma once

#include <functional>

namespace skyfire
{
    class sf_object_meta_run final
    {
    public:
	    explicit sf_object_meta_run(std::function<void()> f);
    };
}