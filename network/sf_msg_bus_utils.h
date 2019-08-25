
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_msg_bus_utils.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#include <string>

#include "core/sf_type.hpp"
#include "tools/sf_json.hpp"

namespace skyfire {

constexpr int msg_bus_reg_type_single = 0;
constexpr int msg_bus_reg_type_multi = 1;
constexpr int msg_bus_new_msg = 2;
constexpr int msg_bus_unreg_single = 3;
constexpr int msg_bus_unreg_multi = 4;

/**
 *   @brief  消息总线数据
 */
struct sf_msg_bus_t final {
    std::string type;
    byte_array data;
};

SF_JSONIFY(sf_msg_bus_t, type, data)

}    // namespace skyfire