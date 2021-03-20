
#pragma once
#include "json.h"
#include "nocopy.h"
#include "stdc++.h"
#include "tcp_client.h"
#include "timer.hpp"
#include "utils.h"
namespace skyfire
{
struct rpc_context_t
{
    pkg_header_t                           header;
    byte_array                             data;
    std::mutex                             back_mu;
    std::condition_variable                back_cond;
    std::atomic<bool>                      back_finished;
    bool                                   is_async;
    std::function<void(const byte_array&)> async_callback;
};
class rpc_ret_t
{
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
class rpc_client
    : public make_instance_t<rpc_client, nocopy<object>>
{
private:
    std::shared_ptr<tcp_client> tcp_client__ = tcp_client::make_instance();
    // TODO 此处使用unordered_map会崩溃，在计算hash的时候会发生除0错误，why？
    std::map<int, std::shared_ptr<rpc_context_t>> rpc_data__;
    int                                           current_call_id__ = 0;
    unsigned int                                  rpc_timeout__     = 30000;
    int                                           make_call_id__();
    void                                          back_callback__(const pkg_header_t& header_t,
                                                                  const byte_array&   data_t);
    void                                          on_closed__();

public:
    rpc_client();
    void set_rpc_timeout(unsigned int ms);
    bool connect_to_server(std::string ip, unsigned short port) const;
    void close() const;
    template <typename... __SF_RPC_ARGS__>
    rpc_ret_t call(const std::string& func_id, __SF_RPC_ARGS__... args);
    template <typename... __SF_RPC_ARGS__>
    void call(const std::string& func_id, std::function<void()> rpc_callback,
              __SF_RPC_ARGS__... args);
    template <typename _Ret, typename... __SF_RPC_ARGS__>
    void call(const std::string&        func_id,
              std::function<void(_Ret)> rpc_callback, __SF_RPC_ARGS__... args);
    friend std::shared_ptr<tcp_client>;
};
#define RPC_OBJECT(name) \
    struct name          \
        : public skyfire::make_instance_t<name, skyfire::rpc_client>
#define RPC_INTERFACE(name)                                                \
    template <typename... __SF_RPC_ARGS__>                                 \
    skyfire::rpc_ret_t name(__SF_RPC_ARGS__&&... args)                     \
    {                                                                      \
        return call(#name, std::forward<__SF_RPC_ARGS__>(args)...);        \
    }                                                                      \
    template <typename _Ret, typename... __SF_RPC_ARGS__>                  \
    void name(std::function<void(_Ret)> rpc_callback,                      \
              __SF_RPC_ARGS__&&... args)                                   \
    {                                                                      \
        call(#name, rpc_callback, std::forward<__SF_RPC_ARGS__>(args)...); \
    }                                                                      \
    template <typename... __SF_RPC_ARGS__>                                 \
    void name(std::function<void()> rpc_callback,                          \
              __SF_RPC_ARGS__&&... args)                                   \
    {                                                                      \
        call(#name, rpc_callback, std::forward<__SF_RPC_ARGS__>(args)...); \
    }
} // namespace skyfire
