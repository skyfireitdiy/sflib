
/**
* @version 1.0.0
* @author skyfire
* @file sf_rpc_client.hpp
*/

/*
 * rpc_client rpc客户端
 */
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic   ignored "OCSimplifyInspection"

#include "sf_json.hpp"
#include "sf_rpc_client.h"
#include "sf_rpc_utils.h"

namespace skyfire
{
inline int rpc_client::make_call_id__()
{
    current_call_id__ = (current_call_id__ + 1) & rpc_req_type;
    return current_call_id__;
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
    sf_debug("async call", skyfire::to_json(req));
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
    sf_debug("async call", skyfire::to_json(req));
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
//    sf_debug("async call 3", call_id);
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
    sf_debug("call", skyfire::to_json(req));
    tcp_client__->send(rpc_req_type,
                       to_byte_array(skyfire::to_json(req).to_string()));
    {
        bool wait_ret {};
        {
            std::unique_lock<std::mutex> lck(rpc_data__[call_id]->back_mu);
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

inline void rpc_client::set_rpc_timeout(unsigned int ms)
{
    rpc_timeout__ = ms;
}

inline rpc_client::rpc_client()
{
    sf_bind(
        tcp_client__, data_coming,
        [this](const pkg_header_t& header_t, const byte_array& data_t) {
            back_callback__(header_t, data_t);
        },
        true);

    sf_bind(
        tcp_client__, closed, [this]() { close(); }, true);
}

inline void rpc_client::close() const { tcp_client__->close(); }

inline bool rpc_client::connect_to_server(const std::string ip,
                                          unsigned short    port) const
{
    return tcp_client__->connect_to_server(ip, port);
}

inline void rpc_client::back_callback__(const pkg_header_t& header_t,
                                        const byte_array&   data_t)
{
    if (header_t.type != rpc_res_type)
    {
        return;
    }
    rpc_res_context_t res;
    from_json(json::from_string(to_string(data_t)), res);
    const auto call_id = res.call_id;
    if (rpc_data__.count(call_id) == 0)
    {
        return;
    }
    sf_debug("call ret", to_string(data_t));
    if (rpc_data__[call_id]->is_async)
    {
        rpc_data__[call_id]->async_callback(data_t);
        rpc_data__.erase(call_id);
    }
    else
    {
        rpc_data__[call_id]->header        = header_t;
        rpc_data__[call_id]->data          = data_t;
        rpc_data__[call_id]->back_finished = true;
        rpc_data__[call_id]->back_cond.notify_one();
    }
}

inline void rpc_client::on_closed__()
{
    for (auto& p : rpc_data__)
    {
        if (!p.second->is_async)
        {
            p.second->back_finished = false;
            p.second->back_cond.notify_one();
        }
    }
}
} // namespace skyfire

#pragma clang diagnostic pop