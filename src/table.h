#pragma once
#include "color.h"
#include "error.h"
#include "stdc++.h"
namespace skyfire
{
struct table_result_t
{
    error_code  ec;
    std::string err_string;
                operator bool() const { return ec == err_ok; }
};
enum class table_align
{
    align_left,
    align_right,
    align_center
};
struct table_column_config_t
{
    table_align        align = table_align::align_left;
    std::vector<color> cl;
    int                min_width = 0;
};
struct table_item_config_t
{
    table_align        align = table_align::align_left;
    std::vector<color> cl;
};
class table
{
private:
    const size_t                                                                        col_num__;
    std::vector<std::string>                                                            header__;
    std::vector<std::vector<std::string>>                                               body__;
    std::function<std::string(size_t, size_t, std::string)>                             proxy_callback__;
    std::vector<table_column_config_t>                                                  table_config__;
    mutable std::unordered_map<size_t, std::unordered_map<size_t, table_item_config_t>> item_config__;
    // mutable std::mutex mu__;
public:
    table(int col_size);
    table_result_t add_row(const std::vector<std::string>& data);
    table_result_t add_rows(const std::vector<std::vector<std::string>>& data);
    table_result_t insert_row(size_t n, const std::vector<std::string>& data);
    table_result_t insert_rows(size_t n, const std::vector<std::vector<std::string>>& data);
    table_result_t delete_rows(size_t n, size_t len = 1);
    table_result_t set_header(const std::vector<std::string>& header);
    table_result_t set_config(size_t c, const table_column_config_t& config);
    table_result_t set_config(const std::vector<table_column_config_t>& config);
    table_result_t set_config(size_t r, size_t c, const table_item_config_t& config);
    void           set_proxy_callback(std::function<std::string(size_t, size_t, std::string)> cb);
    std::string    to_string() const;
};
}