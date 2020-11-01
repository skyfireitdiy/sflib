#pragma once
#include "sf_stdc++.h"

namespace skyfire {

// error_code

enum error_code {
    // 无错误
    err_ok = 0,

    // 资源不存在
    err_not_exist = 0x00000001,

    // 连接断开
    err_disconnect = 0x00000002,

    // 远程错误
    err_remote_err = 0x00000003,

    // 监听错误
    err_listen_err = 0x00000004,

    // 绑定错误
    err_bind_err = 0x00000005,

    // NAT穿透错误
    err_nat_traversal_err = 0x00000006,

    err_finished = 0x00000007,

    // 解析错误
    err_parse = 0x00000008,

    // 不支持的类型
    err_unsupport_type = 0x00000009,

    // 缺少必要参数
    err_not_enough = 0x0000000a,

    // 列数量不匹配
    err_col_count_mismatch = 0x0000000b,

    // 索引越界
    err_index_out_of_range = 0x0000000c,

    // channel 关闭
    err_chan_close = 0x0000000e,

};

/**
 * @brief 异常
 * 
 */

class exception : public std::exception {
private:
    int ec__;
    std::string msg__;
    mutable std::string buf__;

public:
    const char* what() const noexcept override
    {
        buf__ = "ec:" + std::to_string(ec__) + " msg:" + msg__;
        return buf__.c_str();
    }
    int code() const noexcept
    {
        return ec__;
    }
    exception(int ec, std::string msg)
        : ec__(ec)
        , msg__(msg)
    {
    }
};

/**
 * @brief 错误
 * 
 */
class err {
private:
    exception exp__;

public:
    err(exception e = exception(err_ok, ""))
        : exp__(e)
    {
    }
    operator bool() const { return exp__.code() == err_ok; }
    exception exp() const { return exp__; }
};
}