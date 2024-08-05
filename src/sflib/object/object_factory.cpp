
#include "sflib/object/object_factory.h"
#include "sflib/json/json.h"

#include "sflib/tools/utils.h"

namespace skyfire
{
bool object_factory::load_config(const std::string &config_str)
{
    auto config_obj = json::from_string(config_str);
    if (config_obj.is_null())
    {
        return false;
    }
    return load_data__(config_obj);
}
bool object_factory::load_config_file(const std::string &config_file)
{
    byte_array data;
    if (read_file(config_file, data) == false)
    {
        return false;
    }
    return load_config(skyfire::to_string(data));
}
bool object_factory::set_config(const json &config_obj)
{
    return load_data__(config_obj);
}
bool object_factory::load_data__(const json &config_obj)
{
    object_data__.clear();
    auto size = config_obj.size();
    for (size_t i = 0; i < size; ++i)
    {
        object_factory_config_item_t item;
        auto tmp_obj = config_obj.at(i);
        item.id = static_cast<std::string>(tmp_obj["id"]);
        item.data = tmp_obj["data"];
        item.singleton = tmp_obj["singleton"];
        if (item.id.empty())
        {
            object_data__.clear();
            return false;
        }
        object_data__[item.id] = item;
    }
    return true;
}
json object_factory::object_data(const std::string &obj_name)
{
    if (object_data__.count(obj_name) == 0)
    {
        return json();
    }
    auto obj = object_data__[obj_name].data.deep_copy();
    if (obj.has("__base__"))
    {
        auto base_obj = object_data(static_cast<std::string>(obj["__base__"]));
        obj.remove("__base__");
        base_obj.join(obj);
        return base_obj;
    }
    else
    {
        return obj;
    }
}

} // namespace skyfire