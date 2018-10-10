#pragma once

#include "sf_type.h"
#include <string>
#include <map>

namespace skyfire
{
    struct sf_component_context_t
    {
        std::string name;
        SOCKET sock;
        std::map<std::string, byte_array> private_data;
    };
}