
#pragma once
#include "sf_json.h"
namespace skyfire
{
class render_interface
{
public:
    virtual std::string render(const std::string& str, const json& data) = 0;
};
} // namespace skyfire