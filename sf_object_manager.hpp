#pragma once

#include "sf_object_manager.h"
#include "sf_single_instance.hpp"
#include "sf_object_global_meta_info.hpp"

#include "sf_json.hpp"

#include <fstream>

#include <iostream>
using namespace std;

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
        cout<<config<<endl;
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

                    if(properties[p].has("ref"))
                    {
                        tmp_property.type = object_value_type_t ::ref;
                        tmp_property.data = properties[p]["ref"];
                    } else{
                        tmp_property.type = object_value_type_t ::value;
                        tmp_property.data = properties[p]["value"];
                    }
                    tmp_item.properties[p] = tmp_property;
                }
                
                meta__.objects[tmp_item.id]=tmp_item;
            }
        }
        return true;
    }

    template<typename T>
    std::shared_ptr<T> sf_object_manager::get_object(const std::string& key)
    {
        if(meta__.objects.count(key) == 0){
            cout<<"return nullptr"<<endl;
            return nullptr;
        }
        cout<<"has key"<<endl;
        auto item = meta__.objects[key];

        if(item.scope == object_item_scope_t::singleton)
        {
            cout<<"singleton"<<endl;
            if(singleton_data__.count(key)!= 0){
                return std::any_cast<std::shared_ptr<T>>(singleton_data__[key]);
            }
        }
        cout<<"get object"<<endl;
        auto obj = sf_object_global_meta_info::get_instance()->get_object<T>(item._class);
        for(auto &p:item.properties)
        {
            cout<<"set property " + p.first<<endl;
            if(p.second.type == object_value_type_t::value){
                sf_object_global_meta_info::get_instance()->set_value<T>(obj,p.first, p.second.data);
            }
//            else
//            {
//                sf_object_global_meta_info::get_instance()->set_value<T>(obj,p.first, get_object<std::any>(std::static_cast<std::string>(p.second.data)));
//            }
        }
        if(item.scope == object_item_scope_t::singleton)
        {
            singleton_data__[key] = obj;
        }
        return obj;
    }
}