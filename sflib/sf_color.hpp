#pragma once

#include "sf_color.h"

namespace skyfire {
std::string sf_color_string(const std::string& str, const std::vector<sf_color>& cv)
{
    if (cv.empty() || cv.size() > 3) {
        return str;
    }
    std::string prefix = "\033[";
    for (auto& t : cv) {
        prefix += std::to_string(t) + ";";
    }
    prefix[prefix.length() - 1] = 'm';
    std::string suffix = "\033[0m";
    return prefix + str + suffix;
}
}