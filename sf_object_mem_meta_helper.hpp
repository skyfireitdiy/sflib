#pragma once

#include "sf_object_mem_meta_helper.h"
#include "sf_object.hpp"
#include "sf_json.hpp"

#include <iostream>

using namespace std;

namespace skyfire
{
    sf_object_mem_meta_helper::sf_object_mem_meta_helper(sf_object *obj,const std::string& key ,std::function<void(sf_json)> func) {
        cout<<"set function"<<endl;
        obj->member_set_callback__[key] = func;
    }
}