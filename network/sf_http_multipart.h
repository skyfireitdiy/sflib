
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_http_multipart.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include "core/sf_type.h"
#include "sf_http_header.h"

namespace skyfire {

/**
 * @brief  http分块请求数据包
 */
class sf_http_multipart final {
   private:
    sf_http_header header__;
    bool end__ = {false};
    bool finish__ = {false};
    std::string boundary_str__;
    std::shared_ptr<std::ofstream> fp__;
    bool first_block = true;
    std::string filename__;

   public:
    /**
     * 构造函数
     * @param boundary_str 分块边界字符串
     * @param tmp_path 临时文件目录（分块请求会保存至临时目录的文件）
     */
    sf_http_multipart(const std::string& boundary_str,
                      const std::string& tmp_path);
    /**
     * 获取分块请求的头部信息
     * @return 头部信息
     */
    sf_http_header header() const;
    /**
     * 判断是否是结束包（最后一个分块请求结束）
     * @return 是否是结束包
     */
    bool is_end() const;
    /**
     * 判断此分块请求是否结束
     * @return
     * 此分块请求是否结束（后续可能还会有其他分块，如果要判断是否整个请求结束，使用is_end）
     */
    bool is_finished() const;
    /**
     * 获取临时文件名称
     * @return
     * 临时文件名称（注意：临时文件不会自动删除，需要用户在使用完成后手动删除）
     */
    std::string filename() const;
    /**
     * 往当前请求块添加数据
     * @param data 数据
     * @param ret 被解析后剩余的数据
     * @return 是否完整解析
     */
    bool append_data(const byte_array& data, byte_array& ret);
};

}    // namespace skyfire

#pragma clang diagnostic pop