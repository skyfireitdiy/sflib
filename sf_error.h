#pragma once
#include "sf_stdc++.h"

namespace skyfire {

// error_code

enum sf_error_code {
    // 无错误
    sf_err_ok = 0,

    // 资源不存在
    sf_err_not_exist = 0x00010001,

    // 连接断开
    sf_err_disconnect = 0x00010002,

    // 远程错误
    sf_err_remote_err = 0x00010003,

    // 监听错误
    sf_err_listen_err = 0x00010004,

    // 绑定错误
    sf_err_bind_err = 0x00010005,

    // NAT穿透错误
    sf_err_nat_traversal_err = 0x00010005,

    // channel 关闭
    sf_chan_close = 0x00010006,
};

class sf_exception : public std::exception {
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
    sf_exception(int ec, std::string msg)
        : ec__(ec)
        , msg__(msg)
    {
    }
};

}