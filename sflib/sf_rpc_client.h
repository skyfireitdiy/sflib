
/**
* @version 1.0.0
* @author skyfire
* @file sf_rpc_client.h
*/

/*
 * rpc_client rpc客户端
 */
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include "sf_stdc++.h"
#include "sf_timer.hpp"
#include "sf_tcp_client.h"
#include "sf_json.h"
#include "sf_nocopy.h"
#include "sf_utils.h"

namespace skyfire {

/**
 *  @brief rpc数据
 */
struct rpc_context_t {
    pkg_header_t header;
    byte_array data;
    std::mutex back_mu;
    std::condition_variable back_cond;
    std::atomic<bool> back_finished;
    bool is_async;
    std::function<void(const byte_array&)> async_callback;
};

class rpc_ret_t {
    bool valid__;
    json data__;

public:
    rpc_ret_t(bool valid, const json& data)
        : valid__(valid)
        , data__(data)
    {
    }

    bool valid() const { return valid__; }

    template <typename T>
    T get() const
    {
        T ret;
        from_json(data__["ret"], ret);
        return ret;
    }

    template <typename T>
    operator T() const
    {
        return get<T>();
    }
};

/**
 *  @brief rpc客户端类
 */
class rpc_client
    : public make_instance_t<rpc_client, nocopy<object>> {
private:
    std::shared_ptr<tcp_client> tcp_client__ = tcp_client::make_instance();
    // TODO 此处使用unordered_map会崩溃，在计算hash的时候会发生除0错误，why？
    std::map<int, std::shared_ptr<rpc_context_t>> rpc_data__;

    int current_call_id__ = 0;
    unsigned int rpc_timeout__ = 30000;

    int make_call_id__();

    void back_callback__(const pkg_header_t& header_t,
        const byte_array& data_t);

    void on_closed__();

public:
    rpc_client();

    /**
     * @brief set_rpc_timeout 设置RPC超时
     * @param ms 超时毫秒数
     */
    void set_rpc_timeout(unsigned int ms);

    /**
     * @brief connect 连接RPC服务端
     * @param ip ip
     * @param port 端口
     * @return 是否连接成功
     */
    bool connect_to_server(std::string ip, unsigned short port) const;

    /**
     * @brief close 关闭RPC客户端
     */
    void close() const;

    /**
     * @brief call 调用
     * @param func_id 函数id
     * @param args 参数列表
     * @return 返回值
     */
    template <typename... __SF_RPC_ARGS__>
    rpc_ret_t call(const std::string& func_id, __SF_RPC_ARGS__... args);

    /**
     * @brief async_call 异步调用（无返回）
     * @param func_id 函数id
     * @param rpc_callback 返回后的回调函数
     * @param args 参数
     */
    template <typename... __SF_RPC_ARGS__>
    void call(const std::string& func_id, std::function<void()> rpc_callback,
        __SF_RPC_ARGS__... args);

    /**
     * @brief async_call 异步调用（有返回）
     * @param func_id 函数id
     * @param rpc_callback 返回后的回调函数
     * @param args 参数
     */
    template <typename _Ret, typename... __SF_RPC_ARGS__>
    void call(const std::string& func_id,
        std::function<void(_Ret)> rpc_callback, __SF_RPC_ARGS__... args);

    friend std::shared_ptr<tcp_client>;
};

#define RPC_OBJECT(name) \
    struct name          \
        : public skyfire::make_instance_t<name, skyfire::rpc_client>

#define RPC_INTERFACE(name)                                                \
    template <typename... __SF_RPC_ARGS__>                                 \
    skyfire::rpc_ret_t name(__SF_RPC_ARGS__&&... args)                  \
    {                                                                      \
        return call(#name, std::forward<__SF_RPC_ARGS__>(args)...);        \
    }                                                                      \
    template <typename _Ret, typename... __SF_RPC_ARGS__>                  \
    void name(std::function<void(_Ret)> rpc_callback,                      \
        __SF_RPC_ARGS__&&... args)                                         \
    {                                                                      \
        call(#name, rpc_callback, std::forward<__SF_RPC_ARGS__>(args)...); \
    }                                                                      \
    template <typename... __SF_RPC_ARGS__>                                 \
    void name(std::function<void()> rpc_callback,                          \
        __SF_RPC_ARGS__&&... args)                                         \
    {                                                                      \
        call(#name, rpc_callback, std::forward<__SF_RPC_ARGS__>(args)...); \
    }

} // namespace skyfire

#pragma clang diagnostic pop