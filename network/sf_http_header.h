
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_http_header.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include "sf_http_utils.h"

namespace skyfire {

class sf_http_base_server;

/**
 * @brief  http头
 */
class sf_http_header {
    sf_http_header_t header_data__;
    std::vector<std::string> cookie_str_vec__;

   public:
    /**
     * 清空http头
     */
    void clear();

    /**
     * 设置头，注意：不支持同名http头，设置同名http头会将前一个同名http头覆盖
     * @param key 键
     * @param value 值
     */
    void set_header(std::string key, const std::string& value);

    /**
     * 获取头数据
     * @param key 键
     * @param default_value 默认值（如果没有找到指定的http头，则会返回该值）
     * @return 值
     */
    std::string header_value(std::string key,
                             const std::string& default_value = "") const;

    /**
     * 删除指定的头
     * @param key 键
     */
    void remove_header(std::string key);

    /**
     * 获取头的列表
     * @return 头的键集合
     */
    std::vector<std::string> key_list() const;

    /**
     * 获取头的原始类型
     * @return sf_http_header_t类型的头数据
     */
    sf_http_header_t header() const;

    /**
     * 设置原始的头数据
     * @param header sf_http_header_t类型的头数据
     */
    void set_header(const sf_http_header_t& header);

    /**
     * 判断是否有指定的http头
     * @param key 键
     * @return 是否存在
     */
    bool has_key(const std::string& key) const;

    /**
     * 生成符合http标准的字符串格式的头文本，用于发送/查看，注意：cookie不包含在头列表中，但是生成字符串时，会将cookie编入
     * @return 生成的头文本
     */
    std::string to_string() const;

    friend sf_http_base_server;
};
}    // namespace skyfire
#pragma clang diagnostic pop