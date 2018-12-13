
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_rpc_utils.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#include "sf_serialize_binary.hpp"
#include "sf_type.h"
#include "sf_stdc++.h"

namespace skyfire
{
    /**
     *  @brief rpc请求上下文
     */
    struct sf_rpc_req_context_t
    {
        int call_id;                    // 调用id
        std::string func_id;            // 函数id
        byte_array params;              // 参数
    };

    SF_MAKE_SERIALIZABLE_BINARY(sf_rpc_req_context_t, call_id, func_id, params)

    /**
     *  @brief rpc响应上下文
     */
    struct sf_rpc_res_context_t
    {
        int call_id;                    // 调用id
        byte_array ret;                 // 返回值
    };

    SF_MAKE_SERIALIZABLE_BINARY(sf_rpc_res_context_t, call_id, ret)
}