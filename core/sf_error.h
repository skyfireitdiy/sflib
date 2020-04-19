#pragma once
#include "core/sf_stdc++.h"

namespace skyfire {
class sf_chan_close_exception : public std::exception {
public:
    [[nodiscard]] const char* what() const noexcept override
    {
        return "channel closed";
    }
};
}