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
    inline bool sf_object_manager::load_config(const std::string &file_path) {
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
                    tmp_item.properties[p] = properties[p];
                }
                
                meta__.objects[tmp_item.id]=tmp_item;
            }
        }
        return true;
    }

    inline std::shared_ptr<sf_object> sf_object_manager::get_object__(const std::string &key)
    {
        if(meta__.objects.count(key) == 0){
            return nullptr;
        }
        auto item = meta__.objects[key];

        if(item.scope == object_item_scope_t::singleton)
        {
            if(singleton_data__.count(key)!= 0){
                return (singleton_data__[key]);
            }
        }
        auto obj = sf_object_global_meta_info::get_instance()->get_object(item._class);
        obj->__object_id__ = key;

        for(auto &p:item.properties) {
            switch (obj->__get_mem_value_type(p.first)) {
                case sf_object::__mem_value_type_t__::value:
                    sf_object_global_meta_info::get_instance()->set_value(obj, p.first, p.second);
                    break;
                case sf_object::__mem_value_type_t__::ref:
                    sf_object_global_meta_info::get_instance()->set_ref(obj, p.first, get_object__(
                            static_cast<std::string>(std::any_cast<sf_json>(p.second))));
                    break;
                case sf_object::__mem_value_type_t__::pointer:
                    sf_object_global_meta_info::get_instance()->set_pointer(obj, p.first, get_object__(
                            static_cast<std::string>(std::any_cast<sf_json>(p.second))));
                    break;
                case sf_object::__mem_value_type_t__::container_value:
                    sf_object_global_meta_info::get_instance()->set_container_value(obj, p.first, p.second);
                    break;
                case sf_object::__mem_value_type_t__::container_ref: {
                    std::vector<std::shared_ptr<sf_object>> data;
                    auto js = std::any_cast<sf_json>(p.second);
                    int sz = static_cast<int>(js.size());
                    for (int i = 0; i < sz; ++i) {
                        auto tp = get_object__(static_cast<std::string>(js[i]));
                        if (tp) {
                            data.push_back(tp);
                        }
                    }
                    sf_object_global_meta_info::get_instance()->set_container_ref(obj, p.first, data);
                }
                    break;
                case sf_object::__mem_value_type_t__::container_pointer: {
                    std::vector<std::shared_ptr<sf_object>> data;
                    auto js = std::any_cast<sf_json>(p.second);
                    int sz = static_cast<int>(js.size());
                    for (int i = 0; i < sz; ++i) {
                        auto tp = get_object__(static_cast<std::string>(js[i]));
                        if (tp) {
                            data.push_back(tp);
                        }
                    }
                    sf_object_global_meta_info::get_instance()->set_container_pointer(obj, p.first, data);
                }
                    break;
                case sf_object::__mem_value_type_t__::associated_container_base_base:
                    sf_object_global_meta_info::get_instance()->set_associated_container_base_base(obj, p.first,
                                                                                                     p.second);
                    break;
                case sf_object::__mem_value_type_t__::associated_container_base_value: {
                    std::vector<std::pair<std::any, std::shared_ptr<sf_object>>> data;
                    auto js = std::any_cast<sf_json>(p.second);
                    int sz = static_cast<int>(js.size());
                    for (int i = 0; i < sz; ++i) {
                        std::any k = js[i]["key"];
                        auto v = get_object__(static_cast<std::string>(js[i]["value"]));
                        if (v) {
                            data.emplace_back(k, v);
                        }
                    }
                    sf_object_global_meta_info::get_instance()->set_associated_container_base_value(obj, p.first, data);
                }
                    break;
                case sf_object::__mem_value_type_t__::associated_container_base_pointer: {
                    std::vector<std::pair<std::any, std::shared_ptr<sf_object>>> data;
                    auto js = std::any_cast<sf_json>(p.second);
                    int sz = static_cast<int>(js.size());
                    for (int i = 0; i < sz; ++i) {
                        std::any k = js[i]["key"];
                        auto v = get_object__(static_cast<std::string>(js[i]["value"]));
                        if (v) {
                            data.emplace_back(k, v);
                        }
                    }
                    sf_object_global_meta_info::get_instance()->set_associated_container_base_pointer(obj, p.first,
                                                                                                       data);
                }
                    break;
                case sf_object::__mem_value_type_t__::associated_container_value_base: {
                    std::vector<std::pair<std::shared_ptr<sf_object>,std::any>> data;
                    auto js = std::any_cast<sf_json>(p.second);
                    int sz = static_cast<int>(js.size());
                    for (int i = 0; i < sz; ++i) {
                        auto k = get_object__(static_cast<std::string>(js[i]["key"]));
                        std::any v = js[i]["value"];
                        if (k) {
                            data.emplace_back(k, v);
                        }
                    }
                    sf_object_global_meta_info::get_instance()->set_associated_container_value_base(obj, p.first, data);
                }
                    break;
                case sf_object::__mem_value_type_t__ ::associated_container_value_value:{
                    std::vector<std::pair<std::shared_ptr<sf_object>,std::shared_ptr<sf_object>>> data;
                    auto js = std::any_cast<sf_json>(p.second);
                    int sz = static_cast<int>(js.size());
                    for (int i = 0; i < sz; ++i) {
                        auto k = get_object__(static_cast<std::string>(js[i]["key"]));
                        auto v = get_object__(static_cast<std::string>(js[i]["value"]));
                        if (k && v) {
                            data.emplace_back(k, v);
                        }
                    }
                    sf_object_global_meta_info::get_instance()->set_associated_container_value_value(obj, p.first, data);
                }
                    break;
                case sf_object::__mem_value_type_t__ ::associated_container_value_pointer:{
                    std::vector<std::pair<std::shared_ptr<sf_object>,std::shared_ptr<sf_object>>> data;
                    auto js = std::any_cast<sf_json>(p.second);
                    int sz = static_cast<int>(js.size());
                    for (int i = 0; i < sz; ++i) {
                        auto k = get_object__(static_cast<std::string>(js[i]["key"]));
                        auto v = get_object__(static_cast<std::string>(js[i]["value"]));
                        if (k && v) {
                            data.emplace_back(k, v);
                        }
                    }
                    sf_object_global_meta_info::get_instance()->set_associated_container_value_pointer(obj, p.first, data);
                }
                    break;
                case sf_object::__mem_value_type_t__::associated_container_pointer_base: {
                    std::vector<std::pair<std::shared_ptr<sf_object>,std::any>> data;
                    auto js = std::any_cast<sf_json>(p.second);
                    int sz = static_cast<int>(js.size());
                    for (int i = 0; i < sz; ++i) {
                        auto k = get_object__(static_cast<std::string>(js[i]["key"]));
                        std::any v = js[i]["value"];
                        if (k) {
                            data.emplace_back(k, v);
                        }
                    }
                    sf_object_global_meta_info::get_instance()->set_associated_container_pointer_base(obj, p.first, data);
                }
                    break;
                case sf_object::__mem_value_type_t__::associated_container_pointer_value: {
                    std::vector<std::pair<std::shared_ptr<sf_object>,std::shared_ptr<sf_object>>> data;
                    auto js = std::any_cast<sf_json>(p.second);
                    int sz = static_cast<int>(js.size());
                    for (int i = 0; i < sz; ++i) {
                        auto k = get_object__(static_cast<std::string>(js[i]["key"]));
                        auto v = get_object__(static_cast<std::string>(js[i]["value"]));
                        if (k && v) {
                            data.emplace_back(k, v);
                        }
                    }
                    sf_object_global_meta_info::get_instance()->set_associated_container_pointer_value(obj, p.first, data);
                }
                    break;
                case sf_object::__mem_value_type_t__::associated_container_pointer_pointer: {
                    std::vector<std::pair<std::shared_ptr<sf_object>,std::shared_ptr<sf_object>>> data;
                    auto js = std::any_cast<sf_json>(p.second);
                    int sz = static_cast<int>(js.size());
                    for (int i = 0; i < sz; ++i) {
                        auto k = get_object__(static_cast<std::string>(js[i]["key"]));
                        auto v = get_object__(static_cast<std::string>(js[i]["value"]));
                        if (k && v) {
                            data.emplace_back(k, v);
                        }
                    }
                    sf_object_global_meta_info::get_instance()->set_associated_container_pointer_pointer(obj, p.first, data);
                }
                    break;
                default:
                    break;
            }
        }


        if(item.scope == object_item_scope_t::singleton)
        {
            singleton_data__[key] = obj;
        }
        return obj;
    }

    template<typename T>
    std::shared_ptr<T> sf_object_manager::get_object(const std::string &key) {
        return std::dynamic_pointer_cast<T>(get_object__(key));
    }


}