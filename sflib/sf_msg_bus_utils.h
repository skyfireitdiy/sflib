
/**
* @version 1.0.0
* @author skyfire
* @file sf_msg_bus_utils.h
*/

#pragma once

#include <string>

#include "sf_json.hpp"
#include "sf_type.hpp"

namespace skyfire
{

constexpr int msg_bus_reg_type_single = 0;
constexpr int msg_bus_reg_type_multi  = 1;
constexpr int msg_bus_new_msg         = 2;
constexpr int msg_bus_unreg_single    = 3;
constexpr int msg_bus_unreg_multi     = 4;

/**
 *   @brief  消息总线数据
 */
struct msg_bus_t final
{
    std::string type;
    byte_array  data;
};

SF_JSONIFY(msg_bus_t, type, data)

} // namespace skyfire