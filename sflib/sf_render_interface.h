
/**
* @version 1.0.0
* @author skyfire
* @file sf_render_interface.h

* 
*/

#pragma once

#include "sf_json.h"

namespace skyfire
{
/**
 * @brief 渲染引擎接口
 *
 */
class render_interface
{
public:
    /**
     * @brief 渲染接口
     *
     * @param str 字符串
     * @param data 渲染数据
     * @return std::string 渲染后的字符串
     */
    virtual std::string render(const std::string& str, const json& data) = 0;
};
} // namespace skyfire