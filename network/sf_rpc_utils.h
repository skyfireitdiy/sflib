
/**
* @version 1.0.0
* @author skyfire
* @file sf_rpc_utils.h
*/

#pragma once

#include "core/sf_type.h"
#include "tools/sf_json.h"

namespace skyfire {
/**
 *  @brief rpc请求上下文
 */
struct sf_rpc_req_context_t final {
    int call_id;            // 调用id
    std::string func_id;    // 函数id
    sf_json params;         // 参数
};

SF_JSONIFY(sf_rpc_req_context_t, call_id, func_id, params)

/**
 *  @brief rpc响应上下文
 */
struct sf_rpc_res_context_t final {
    int call_id;    // 调用id
    sf_json ret;    // 返回值
};

SF_JSONIFY(sf_rpc_res_context_t, call_id, ret)
}    // namespace skyfire