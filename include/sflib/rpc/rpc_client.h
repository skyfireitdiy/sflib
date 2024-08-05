
#pragma once

#include "sflib/json/json.h"
#include "sflib/object/timer.h"
#include "sflib/rpc/rpc_utils.h"
#include "sflib/tcp/tcp_client.h"
#include "sflib/tools/nocopy.h"
#include "sflib/tools/utils.h"

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

template <typename _Ret, typename... __SF_RPC_ARGS__>
void rpc_client::call(const std::string&        func_id,
                      std::function<void(_Ret)> rpc_callback,
                      __SF_RPC_ARGS__... args)
{
    static_assert(!std::is_reference<_Ret>::value, "Param can't be reference");
    static_assert(!std::is_pointer<_Ret>::value, "Param can't be pointer");
    static_assert(
        !std::disjunction<std::is_reference<__SF_RPC_ARGS__>...>::value,
        "Param can't be reference");
    static_assert(!std::disjunction<std::is_pointer<__SF_RPC_ARGS__>...>::value,
                  "Param can't be pointer");
    using __Ret = typename std::decay<_Ret>::type;
    std::tuple<typename std::decay<__SF_RPC_ARGS__>::type...> param { args... };
    const auto                                                call_id = make_call_id__();
    rpc_data__[call_id]                                               = std::make_shared<rpc_context_t>();
    rpc_data__[call_id]->is_async                                     = true;
    rpc_data__[call_id]->async_callback                               = [=](const byte_array& data) {
        __Ret             ret;
        rpc_res_context_t res;
        from_json(json::from_string(to_string(data)), res);
        from_json(res.ret, ret);
        rpc_callback(ret);
    };
    rpc_req_context_t req;
    req.call_id = call_id;
    req.func_id = func_id;
    req.params  = skyfire::to_json(param);

    tcp_client__->send(rpc_req_type,
                       to_byte_array(skyfire::to_json(req).to_string()));
    auto p_timer = std::make_shared<timer>();
    sf_bind(p_timer, timeout, ([this, call_id]() {
                if (rpc_data__.count(call_id) != 0)
                {
                    rpc_data__[call_id]->back_cond.notify_one();
                    rpc_data__.erase(call_id);
                }
            }),
            true);
    p_timer->start(rpc_timeout__, true);
}
template <typename... __SF_RPC_ARGS__>
void rpc_client::call(const std::string&    func_id,
                      std::function<void()> rpc_callback,
                      __SF_RPC_ARGS__... args)
{
    static_assert(
        !std::disjunction<std::is_reference<__SF_RPC_ARGS__>...>::value,
        "Param can't be reference");
    static_assert(!std::disjunction<std::is_pointer<__SF_RPC_ARGS__>...>::value,
                  "Param can't be pointer");
    std::tuple<typename std::decay<__SF_RPC_ARGS__>::type...> param { args... };
    const auto                                                call_id = make_call_id__();
    rpc_data__[call_id]                                               = std::make_shared<rpc_context_t>();
    rpc_data__[call_id]->is_async                                     = true;
    rpc_data__[call_id]->async_callback                               = [=](const byte_array& data) {
        rpc_callback();
    };
    rpc_req_context_t req;
    req.call_id = call_id;
    req.func_id = func_id;
    req.params  = to_json(param);

    tcp_client__->send(rpc_req_type,
                       to_byte_array(skyfire::to_json(req).to_string()));
    auto p_timer = std::make_shared<timer>();
    sf_bind(p_timer, timeout, ([this, call_id]() {
                if (rpc_data__.count(call_id) != 0)
                {
                    rpc_data__[call_id]->back_cond.notify_one();
                    rpc_data__.erase(call_id);
                }
            }),
            true);
    p_timer->start(rpc_timeout__, true);
}
// template<typename T>
// void rpc_client::async_call(const std::string &func_id,
// std::function<void()> rpc_callback) {
// const auto call_id = make_call_id__();
//
//    rpc_data__[call_id] = std::make_shared<rpc_context_t>();
//    rpc_data__[call_id]->is_async = true;
//    rpc_data__[call_id]->async_callback = [=](const byte_array &data) {
//        rpc_callback();
//    };
//    rpc_req_context_t req;
//    req.call_id = call_id;
//    req.func_id = func_id;
//    req.params = skyfire::to_json(byte_array());
//    tcp_client__->send(rpc_req_type,
//    to_byte_array(skyfire::to_json(req).to_string())); auto p_timer =
//    std::make_shared<timer>(); sf_bind(p_timer, timeout, [=]() {
//        if(rpc_data__.count(call_id)!=0)
//        {
//            rpc_data__[call_id]->back_cond.notify_one();
//            rpc_data__.erase(call_id);
//        }
//    }, true);
//    p_timer->start(rpc_timeout__, true);
//}
template <typename... __SF_RPC_ARGS__>
rpc_ret_t rpc_client::call(const std::string& func_id,
                           __SF_RPC_ARGS__... args)
{
    static_assert(
        !std::disjunction<std::is_reference<__SF_RPC_ARGS__>...>::value,
        "Param can't be reference");
    static_assert(!std::disjunction<std::is_pointer<__SF_RPC_ARGS__>...>::value,
                  "Param can't be pointer");
    std::tuple<typename std::decay<__SF_RPC_ARGS__>::type...> param { args... };
    const auto                                                call_id = make_call_id__();
    rpc_data__[call_id]                                               = std::make_shared<rpc_context_t>();
    rpc_data__[call_id]->is_async                                     = false;
    rpc_req_context_t req;
    req.call_id = call_id;
    req.func_id = func_id;
    req.params  = skyfire::to_json(param);

    tcp_client__->send(rpc_req_type,
                       to_byte_array(skyfire::to_json(req).to_string()));
    {
        bool wait_ret {};
        {
            std::unique_lock lck(rpc_data__[call_id]->back_mu);
            wait_ret = rpc_data__[call_id]->back_cond.wait_for(
                lck, std::chrono::milliseconds(rpc_timeout__),
                [&]() -> bool { return rpc_data__[call_id]->back_finished; });
        }
        if (!wait_ret)
        {
            rpc_data__.erase(call_id);
            json ret;
            return { false, json() };
        }
    }
    // 连接断开
    if (!rpc_data__[call_id]->back_finished)
    {
        return { false, json() };
    }
    return { true, json::from_string(to_string(rpc_data__[call_id]->data)) };
}

} // namespace skyfire
