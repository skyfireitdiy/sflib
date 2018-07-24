#pragma once

#include <string>

#include "sf_type.h"
#include "sf_serialize_binary.h"

namespace skyfire
{

    constexpr int msg_bus_reg_type_single = 0;
    constexpr int msg_bus_reg_type_multi = 1;
    constexpr int msg_bus_new_msg = 2;
    constexpr int msg_bus_unreg_single = 3;
    constexpr int msg_bus_unreg_multi = 4;


    struct msg_bus_t
    {
        std::string type;
        byte_array data;
    };

    template<>
    byte_array sf_serialize_binary(const msg_bus_t &value)
    {
        return sf_serialize_binary(value.type) + sf_serialize_binary(value.data);
    }

    size_t sf_deserialize_binary(const byte_array &data, msg_bus_t &obj,
                          size_t begin_pos)
    {
        auto ret = sf_deserialize_binary(data, obj.type, begin_pos);
        ret = sf_deserialize_binary(data, obj.data, ret);
        return ret;
    }

}