#pragma once
#include "sf_string.hpp"
#include "sf_table.h"

namespace skyfire {
sf_table::sf_table(int col_size)
    : col_num__(col_size)
    , table_config__(col_size)
{
}

sf_table_result_t sf_table::add_row(const std::vector<std::string>& data)
{
    if (data.size() != col_num__) {
        return sf_table_result_t {
            sf_col_count_mismatch,
            "column count mismatch, want:" + std::to_string(col_num__) + " give:" + std::to_string(sizeof(data.size()))
        };
    }
    body__.push_back(data);
    return sf_table_result_t {
        sf_err_ok,
        ""
    };
}

sf_table_result_t sf_table::add_rows(const std::vector<std::vector<std::string>>& data)
{
    for (auto& p : data) {
        if (p.size() != col_num__) {
            return sf_table_result_t {
                sf_col_count_mismatch,
                "column count mismatch, want:" + std::to_string(col_num__) + " give:" + std::to_string(sizeof(p.size()))
            };
        }
    }
    body__.insert(body__.end(), data.begin(), data.end());
    return sf_table_result_t {
        sf_err_ok,
        ""
    };
}

sf_table_result_t sf_table::insert_row(size_t n, const std::vector<std::string>& data)
{
    if (data.size() != col_num__) {
        return sf_table_result_t {
            sf_col_count_mismatch,
            "column count mismatch, want:" + std::to_string(col_num__) + " give:" + std::to_string(sizeof(data.size()))
        };
    }
    if (n > body__.size()) {
        return sf_table_result_t {
            sf_index_out_of_range,
            "index out of range,max:" + std::to_string(body__.size()) + " give:" + std::to_string(n)
        };
    }
    body__.insert(body__.begin() + n, data);
    return sf_table_result_t {
        sf_err_ok,
        ""
    };
}

sf_table_result_t sf_table::insert_rows(size_t n, const std::vector<std::vector<std::string>>& data)
{
    for (auto& p : data) {
        if (p.size() != col_num__) {
            return sf_table_result_t {
                sf_col_count_mismatch,
                "column count mismatch, want:" + std::to_string(col_num__) + " give:" + std::to_string(sizeof(p.size()))
            };
        }
    }
    if (n > body__.size()) {
        return sf_table_result_t {
            sf_index_out_of_range,
            "index out of range,max:" + std::to_string(body__.size()) + " give:" + std::to_string(n)
        };
    }
    body__.insert(body__.begin() + n, data.begin(), data.end());
    return sf_table_result_t {
        sf_err_ok,
        ""
    };
}

sf_table_result_t sf_table::delete_rows(size_t n, size_t len)
{
    if (n >= body__.size()) {
        return sf_table_result_t {
            sf_index_out_of_range,
            "index out of range,max:" + std::to_string(body__.size() - 1) + " give:" + std::to_string(n)
        };
    }
    if (body__.size() - n < len) {
        return sf_table_result_t {
            sf_index_out_of_range,
            "index out of range,max:" + std::to_string(body__.size() - n) + " give:" + std::to_string(n + len)
        };
    }
    body__.erase(body__.begin() + n, body__.begin() + n + len);
    return sf_table_result_t {
        sf_err_ok,
        ""
    };
}

sf_table_result_t sf_table::set_header(const std::vector<std::string>& header)
{
    if (header.size() != col_num__) {
        return sf_table_result_t {
            sf_col_count_mismatch,
            "column count mismatch, want:" + std::to_string(col_num__) + " give:" + std::to_string(sizeof(header.size()))
        };
    }
    header__ = header;
    return sf_table_result_t {
        sf_err_ok,
        ""
    };
}

sf_table_result_t sf_table::set_config(size_t c, const sf_table_column_config_t& config)
{
    if (c >= col_num__) {
        return sf_table_result_t {
            sf_index_out_of_range,
            "index out of range,max:" + std::to_string(col_num__ - 1) + " give:" + std::to_string(c)
        };
    }
    table_config__[c] = config;
    return sf_table_result_t {
        sf_err_ok,
        ""
    };
}

sf_table_result_t sf_table::set_config(const std::vector<sf_table_column_config_t>& config)
{
    if (config.size() != col_num__) {
        return sf_table_result_t {
            sf_col_count_mismatch,
            "column count mismatch, want:" + std::to_string(col_num__) + " give:" + std::to_string(sizeof(config.size()))
        };
    }
    table_config__ = config;
    return sf_table_result_t {
        sf_err_ok,
        ""
    };
}

void sf_table::set_proxy_callback(std::function<std::string(size_t, size_t, std::string)> cb)
{
    proxy_callback__ = cb;
}

inline std::string sf_full_string(const std::string& str, sf_table_align align, int space_count)
{
    switch (align) {
    case sf_table_align::align_left:
        return str + sf_string::repeat(" ", space_count);
    case sf_table_align::align_center:
        return sf_string::repeat(" ", space_count / 2) + str + sf_string::repeat(" ", space_count - space_count / 2);
    case sf_table_align::align_right:
        return sf_string::repeat(" ", space_count) + str;
    default:
        throw sf_exception(sf_parse_unsupport_type_err, "unsupport type:" + std::to_string(static_cast<int>(align)));
        break;
    }
}

inline std::string sf_table_make_column_string(std::string str, const sf_table_column_config_t& config, int width)
{
    int space_count = width - str.length();
    if (!config.color.empty()) {
        str = sf_color_string(str, config.color);
    }
    return sf_full_string(str, config.align, space_count);
}

std::string sf_table::to_string() const
{
    std::string ret;
    std::vector<int> width_vec(col_num__);
    std::vector<std::string> header = header__;
    std::vector<std::vector<std::string>> body;
    if (proxy_callback__) {
        body.resize(body__.size());
        for (int i = 0; i < body__.size(); ++i) {
            body[i].resize(col_num__);
            for (int j = 0; j < col_num__; ++j) {
                body[i][j] = proxy_callback__(i, j, body__[i][j]);
            }
        }
    } else {
        body = body__;
    }

    for (int i = 0; i < col_num__; ++i) {
        width_vec[i] = table_config__[i].min_width;
    }

    if (!header.empty()) {
        for (int i = 0; i < header.size(); ++i) {
            if (width_vec[i] < header[i].length()) {
                width_vec[i] = header[i].length();
            }
        }
    }
    for (auto& p : body) {
        for (int i = 0; i < col_num__; ++i) {
            if (width_vec[i] < p[i].length()) {
                width_vec[i] = p[i].length();
            }
        }
    }
    if (!header.empty()) {
        for (int i = 0; i < col_num__; ++i) {
            header[i] = sf_full_string(header[i], table_config__[i].align, width_vec[i]-header[i].length());
        }
    }
    for (auto& p : body) {
        for (int i = 0; i < col_num__; ++i) {
            p[i] = sf_table_make_column_string(p[i], table_config__[i], width_vec[i]);
        }
    }

    std::string split_line = "+";
    for (auto& p : width_vec) {
        split_line += sf_string::repeat("-", p) + "+";
    }
    split_line += "\n";

    auto make_row = [](auto& row) {
        std::string r;
        r += "|";
        for (auto& p : row) {
            r += p + "|";
        }
        r += "\n";
        return r;
    };

    if (!header.empty()) {
        ret += split_line;
        ret += make_row(header);
    }

    ret += split_line;

    for (auto& p : body) {
        ret += make_row(p);
    }

    ret += split_line;
    return ret;
}
}