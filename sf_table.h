#pragma once

#include "sf_stdc++.h"
#include "sf_error.h"
#include "sf_color.h"

namespace skyfire {

    struct sf_table_result_t{
        sf_error_code ec;
        std::string err_string;
        operator bool() const{ return ec == sf_err_ok;}
    };

    enum class sf_table_align{
        align_left,
        align_right,
        align_center
    };

    struct sf_table_column_config_t {
        sf_table_align align = sf_table_align::align_left;
        std::vector<sf_color> color;
        int min_width = 0;
    };

    class sf_table{
        private:
        const int col_num__;
        std::vector<std::string> header__;
        std::vector<std::vector<std::string>> body__;
        std::function<std::string(size_t, size_t, std::string)> proxy_callback__;
        std::vector<sf_table_column_config_t> table_config__;
        // mutable std::mutex mu__;
        public:
        /**
         * @brief Construct a new sf table object
         * 
         * @param col_size 列数
         */
        sf_table(int col_size);
        /**
         * @brief 添加一行数据
         * 
         * @param data 一行数据
         * @return sf_table_result_t 结果
         */
        sf_table_result_t add_row(const std::vector<std::string>& data);
        /**
         * @brief 添加多行数据
         * 
         * @param data 多行数据
         * @return sf_table_result_t 结果
         */
        sf_table_result_t add_rows(const std::vector<std::vector<std::string>>& data);
        /**
         * @brief 指定位置插入行
         * 
         * @param n 位置
         * @param data 数据
         * @return sf_table_result_t 结果
         */
        sf_table_result_t insert_row(size_t n,const std::vector<std::string>& data);
        /**
         * @brief 指定位置插入多行
         * 
         * @param n 位置
         * @param data 数据
         * @return sf_table_result_t 结果
         */
        sf_table_result_t insert_rows(size_t n, const std::vector<std::vector<std::string>>& data);
        /**
         * @brief 删除行
         * 
         * @param n 位置
         * @param len 长度
         * @return sf_table_result_t 结果
         */
        sf_table_result_t delete_rows(size_t n, size_t len = 1);

        /**
         * @brief 设置头
         * 
         * @param header 头数据
         * @return sf_table_result_t 结果
         */
        sf_table_result_t set_header(const std::vector<std::string> & header);
        /**
         * @brief 设置列配置
         * 
         * @param c 列
         * @param config 配置
         * @return sf_table_result_t 结果
         */
        sf_table_result_t set_config(size_t c, const sf_table_column_config_t& config);
        /**
         * @brief 设置所有列的配置
         * 
         * @param config 配置
         * @return sf_table_result_t 结果
         */
        sf_table_result_t set_config(const std::vector<sf_table_column_config_t>& config);
        /**
         * @brief 设置代理回调函数
         * @param cb 回调函数
         * 
         */
        void set_proxy_callback(std::function<std::string(size_t, size_t, std::string)> cb);

        /**
         * @brief 转为字符串
         * 
         * @return std::string 字符串
         */
        std::string to_string() const;
    };
}