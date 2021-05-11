#pragma once
#include <bits/stdc++.h>
namespace skyfire
{
// error_code
enum error_code
{
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
    // 结束
    err_finished = 0x00000007,
    // 解析错误
    err_parse = 0x00000008,
    // 不支持的类型
    err_unsupported_type = 0x00000009,
    // 缺少必要参数
    err_not_enough = 0x0000000a,
    // 列数量不匹配
    err_col_count_mismatch = 0x0000000b,
    // 索引越界
    err_index_out_of_range = 0x0000000c,
    // channel 关闭
    err_chan_close = 0x0000000e,
    // dns 解析失败
    err_resolve_dns = 0x0000000f,
    // 连接失败
    err_connect = 0x00000010,
    // 未初始化
    err_uninit = 0x00000011,
    // 流状态错误
    err_stream = 0x00000012,
    // 已存在
    err_exist = 0x00000013,

};
class exception : public std::exception
{
private:
    int                 ec__;
    std::string         msg__;
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
class sf_error
{
private:
    exception exp__;
    std::any  err_info__;

public:
    sf_error(exception e = exception(err_ok, ""), std::any&& err_info = nullptr)
        : exp__(e)
        , err_info__(err_info)
    {
    }
    sf_error(int code, std::string msg, std::any&& err_info = nullptr)
        : sf_error(exception(code, msg))
    {
        err_info__ = std::move(err_info);
    }
    operator bool() const { return exp__.code() == err_ok; }
    exception exp() const { return exp__; }
    std::any  err_info() const { return err_info__; }
};
}