
#pragma once
#include "sf_json.h"
#include "sf_nocopy.h"
#include "sf_object_factory_utils.h"
#include "sf_stdc++.h"
namespace skyfire
{
class object_factory final : public nocopy<>
{
public:
    bool load_config(const std::string& config_str);
    bool load_config_file(const std::string& config_file);
    bool set_config(const json& config_obj);
    template <typename T, typename... ARGS>
    std::shared_ptr<T> object(const std::string& obj_id, ARGS&&... args);

private:
    std::unordered_map<std::string, object_factory_config_item_t>
         object_data__;
    bool load_data__(const json& config_obj);
    json object_data(const std::string& obj_name);
};
} // namespace skyfire