#pragma once

#include "sf_serialize_binary.hpp"
#include "sf_type.h"
#include <string>

namespace skyfire
{
    struct sf_rpc_req_context_t
    {
        int call_id;
        std::string func_id;
        byte_array params;
    };

    SF_MAKE_SERIALIZABLE_BINARY(sf_rpc_req_context_t, call_id, func_id, params)

    struct sf_rpc_res_context_t
    {
        int call_id;
        byte_array ret;
    };

    SF_MAKE_SERIALIZABLE_BINARY(sf_rpc_res_context_t, call_id, ret)
}