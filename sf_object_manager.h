#pragma once

#include "sf_single_instance.h"
#include "sf_nocopy.h"
#include "sf_object_manager_utils.h"

#include <memory>

namespace skyfire
{
    class sf_object_manager : public sf_nocopy<>
    {
    private:
        object_manager_meta_t meta__;
        
    public:
        bool load_config(const std::string &file_path);
    };
}