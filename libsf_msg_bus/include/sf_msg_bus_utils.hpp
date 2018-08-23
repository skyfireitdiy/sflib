#pragma once

#include "sf_msg_bus_utils.h"

namespace skyfire
{
    inline byte_array sf_serialize_binary(const msg_bus_t &value) {
        return sf_serialize_binary(value.type) + sf_serialize_binary(value.data);
    }

    inline size_t sf_deserialize_binary(const byte_array &data, msg_bus_t &obj, size_t begin_pos) {
        auto ret = sf_deserialize_binary(data, obj.type, begin_pos);
        ret = sf_deserialize_binary(data, obj.data, ret);
        return ret;
    }

}