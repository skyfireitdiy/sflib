#pragma once

#include "sf_empty_class.h"

namespace skyfire
{
    template <typename _BaseClass = sf_empty_class>
    class sf_nocopy : public _BaseClass
    {
    public:
        sf_nocopy();
        sf_nocopy(const sf_nocopy&) = delete;
        sf_nocopy(sf_nocopy&&) = delete;
        sf_nocopy& operator=(const sf_nocopy&) = delete;
    };
}