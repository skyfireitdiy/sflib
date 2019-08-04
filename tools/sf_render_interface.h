#pragma once

#include "tools/sf_json.h"

namespace skyfire
{
    class sf_render_interface
    {
    public:
        virtual render(const std::string& str, const sf_json &data) = 0;
    };
}