
#pragma once
#include "sflib/core/type.h"
#include "sflib/json/json.h"

namespace skyfire
{
struct rpc_req_context_t final
{
    int         call_id; // 调用id
    std::string func_id; // 函数id
    json        params;  // 参数
};
SF_JSONIFY(rpc_req_context_t, call_id, func_id, params)
struct rpc_res_context_t final
{
    int  call_id; // 调用id
    json ret;     // 返回值
};
SF_JSONIFY(rpc_res_context_t, call_id, ret)
} // namespace skyfire