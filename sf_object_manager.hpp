#pragma once

#include "sf_object_manager.h"
#include "sf_single_instance.hpp"

#include "sf_json.hpp"

#include <fstream>

namespace skyfire
{
    bool sf_object_manager::load_config(const std::string &file_path) {
        std::ifstream fi(file_path);
        if(!fi) {
            return false;
        }
        std::string file_content;
        std::string tmp;
        while(std::getline(fi,tmp)) {
            file_content += tmp;
        }
        fi.close();
        auto config = sf_json::from_string(file_content);
        if(config.is_null())
        {
            return false;
        }
        if(config.has("objects"))
        {
            auto items = config["objects"];
            auto item_size = items.size();
            for(auto i = 0;i<item_size;++i)
            {
                object_item_meta_t tmp_item;
                tmp_item.id = (std::string)items[i]["id"];
                tmp_item._class = (std::string) items[i]["class"];
                tmp_item.scope = (std::string)items[i]["scope"] == "prototype" ? object_item_scope_t ::prototype : object_item_scope_t ::singleton;
                
                auto properties = items[i]["property"];
                auto keys = properties.keys();
                for(auto &p:keys)
                {
                    object_property_t tmp_property;
                    tmp_property.type =(std::string) properties[p]["type"] == "ref" ? object_value_type_t ::ref:object_value_type_t ::value;
                    tmp_property.data = properties[p]["data"];
                    tmp_item.properties[p] = tmp_property;
                }
                
                meta__.objects.push_back(tmp_item);
            }
        }
        return true;
    }
}