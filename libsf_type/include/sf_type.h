#pragma once

#include <vector>


#ifndef _WIN32
#define SOCKET int
#endif


namespace skyfire
{
    using byte_array = std::vector<char>;

    std::string to_string(const byte_array& data)
    {
        return std::string(data.begin(),data.end());
    }
}