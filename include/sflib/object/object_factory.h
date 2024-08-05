
#pragma once
#include "sflib/json/json.h"
#include "sflib/object/object_factory_utils.h"
#include "sflib/tools/nocopy.h"

namespace skyfire
{
class object_factory final : public nocopy<>
{
public:
    bool load_config(const std::string &config_str);
    bool load_config_file(const std::string &config_file);
    bool set_config(const json &config_obj);
    template <typename T, typename... ARGS>
    std::shared_ptr<T> object(const std::string &obj_id, ARGS &&...args);

private:
    std::unordered_map<std::string, object_factory_config_item_t>
        object_data__;
    bool load_data__(const json &config_obj);
    json object_data(const std::string &obj_name);
};

template <typename T, typename... ARGS>
std::shared_ptr<T> object_factory::object(const std::string &obj_id,
                                          ARGS &&...args)
{
    if (object_data__.count(obj_id) == 0)
    {
        return nullptr;
    }
    auto &tmp_config = object_data__[obj_id];
    if (tmp_config.singleton)
    {
        if (tmp_config.object.has_value())
        {
            return std::any_cast<std::shared_ptr<T>>(tmp_config.object);
        }
    }
    std::shared_ptr<T> obj;
    obj = std::make_shared<T>(std::forward<ARGS>(args)...);
    auto obj_data = object_data(obj_id);
    from_json(obj_data, *obj);
    tmp_config.object = obj;
    return obj;
}
} // namespace skyfire