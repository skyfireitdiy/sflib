
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_rpc_client.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * sf_rpc_client rpc客户端
 */
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "OCSimplifyInspection"

#include "sf_rpc_client.h"
#include "sf_rpc_utils.h"
#include "tools/sf_json.hpp"

namespace skyfire {
inline int sf_rpc_client::__make_call_id() {
    current_call_id__ = (current_call_id__ + 1) & rpc_req_type;
    return current_call_id__;
}

template <typename _Ret, typename... __SF_RPC_ARGS__>
void sf_rpc_client::call(const std::string &func_id,
                         std::function<void(_Ret)> rpc_callback,
                         __SF_RPC_ARGS__... args) {
    static_assert(!std::is_reference<_Ret>::value, "Param can't be reference");
    static_assert(!std::is_pointer<_Ret>::value, "Param can't be pointer");
    static_assert(
        !std::disjunction<std::is_reference<__SF_RPC_ARGS__>...>::value,
        "Param can't be reference");
    static_assert(!std::disjunction<std::is_pointer<__SF_RPC_ARGS__>...>::value,
                  "Param can't be pointer");

    using __Ret = typename std::decay<_Ret>::type;
    std::tuple<typename std::decay<__SF_RPC_ARGS__>::type...> param{args...};
    const auto call_id = __make_call_id();
    __rpc_data__[call_id] = std::make_shared<sf_rpc_context_t>();
    __rpc_data__[call_id]->is_async = true;
    __rpc_data__[call_id]->async_callback = [=](const byte_array &data) {
        __Ret ret;
        sf_rpc_res_context_t res;
        from_json(sf_json::from_string(to_string(data)), res);
        from_json(res.ret, ret);
        rpc_callback(ret);
    };
    sf_rpc_req_context_t req;
    req.call_id = call_id;
    req.func_id = func_id;
    req.params = skyfire::to_json(param);
    sf_debug("async call", skyfire::to_json(req));
    __tcp_client__->send(rpc_req_type,
                         to_byte_array(skyfire::to_json(req).to_string()));
    auto p_timer = std::make_shared<sf_timer>();
    sf_bind_signal(p_timer, timeout, ([this, call_id]() {
                       if (__rpc_data__.count(call_id) != 0) {
                           __rpc_data__[call_id]->back_cond.notify_one();
                           __rpc_data__.erase(call_id);
                       }
                   }),
                   true);
    p_timer->start(rpc_timeout__, true);
}

template <typename... __SF_RPC_ARGS__>
void sf_rpc_client::call(const std::string &func_id,
                         std::function<void()> rpc_callback,
                         __SF_RPC_ARGS__... args) {
    static_assert(
        !std::disjunction<std::is_reference<__SF_RPC_ARGS__>...>::value,
        "Param can't be reference");
    static_assert(!std::disjunction<std::is_pointer<__SF_RPC_ARGS__>...>::value,
                  "Param can't be pointer");
    std::tuple<typename std::decay<__SF_RPC_ARGS__>::type...> param{args...};
    const auto call_id = __make_call_id();
    __rpc_data__[call_id] = std::make_shared<sf_rpc_context_t>();
    __rpc_data__[call_id]->is_async = true;
    __rpc_data__[call_id]->async_callback = [=](const byte_array &data) {
        rpc_callback();
    };
    sf_rpc_req_context_t req;
    req.call_id = call_id;
    req.func_id = func_id;
    req.params = to_json(param);
    sf_debug("async call", skyfire::to_json(req));
    __tcp_client__->send(rpc_req_type,
                         to_byte_array(skyfire::to_json(req).to_string()));
    auto p_timer = std::make_shared<sf_timer>();
    sf_bind_signal(p_timer, timeout, ([this, call_id]() {
                       if (__rpc_data__.count(call_id) != 0) {
                           __rpc_data__[call_id]->back_cond.notify_one();
                           __rpc_data__.erase(call_id);
                       }
                   }),
                   true);
    p_timer->start(rpc_timeout__, true);
}

// template<typename T>
// void sf_rpc_client::async_call(const std::string &func_id,
// std::function<void()> rpc_callback) {
// const auto call_id = __make_call_id();
//    sf_debug("async call 3", call_id);
//    __rpc_data__[call_id] = std::make_shared<sf_rpc_context_t>();
//    __rpc_data__[call_id]->is_async = true;
//    __rpc_data__[call_id]->async_callback = [=](const byte_array &data) {
//        rpc_callback();
//    };
//    sf_rpc_req_context_t req;
//    req.call_id = call_id;
//    req.func_id = func_id;
//    req.params = skyfire::to_json(byte_array());
//    __tcp_client__->send(rpc_req_type,
//    to_byte_array(skyfire::to_json(req).to_string())); auto p_timer =
//    std::make_shared<sf_timer>(); sf_bind_signal(p_timer, timeout, [=]() {
//        if(__rpc_data__.count(call_id)!=0)
//        {
//            __rpc_data__[call_id]->back_cond.notify_one();
//            __rpc_data__.erase(call_id);
//        }
//    }, true);
//    p_timer->start(rpc_timeout__, true);
//}

template <typename... __SF_RPC_ARGS__>
sf_rpc_ret_t sf_rpc_client::call(const std::string &func_id,
                                 __SF_RPC_ARGS__... args) {
    static_assert(
        !std::disjunction<std::is_reference<__SF_RPC_ARGS__>...>::value,
        "Param can't be reference");
    static_assert(!std::disjunction<std::is_pointer<__SF_RPC_ARGS__>...>::value,
                  "Param can't be pointer");

    std::tuple<typename std::decay<__SF_RPC_ARGS__>::type...> param{args...};
    const auto call_id = __make_call_id();
    __rpc_data__[call_id] = std::make_shared<sf_rpc_context_t>();
    __rpc_data__[call_id]->is_async = false;

    sf_rpc_req_context_t req;
    req.call_id = call_id;
    req.func_id = func_id;
    req.params = skyfire::to_json(param);
    sf_debug("call", skyfire::to_json(req));
    __tcp_client__->send(rpc_req_type,
                         to_byte_array(skyfire::to_json(req).to_string()));
    std::cout << "1" << std::endl;
    {
        bool wait_ret{};
        {
            std::unique_lock<std::mutex> lck(__rpc_data__[call_id]->back_mu);
            wait_ret = __rpc_data__[call_id]->back_cond.wait_for(
                lck, std::chrono::milliseconds(rpc_timeout__),
                [&]() -> bool { return __rpc_data__[call_id]->back_finished; });
        }
        if (!wait_ret) {
            __rpc_data__.erase(call_id);
            sf_json ret;
            return {false, sf_json()};
        }
    }
    std::cout << "6" << std::endl;

    // 连接断开
    if (!__rpc_data__[call_id]->back_finished) {
        return {false, sf_json()};
    }
    return {true, sf_json::from_string(to_string(__rpc_data__[call_id]->data))};
}

inline void sf_rpc_client::set_rpc_timeout(unsigned int ms) {
    rpc_timeout__ = ms;
}

inline sf_rpc_client::sf_rpc_client() {
    sf_bind_signal(
        __tcp_client__, data_coming,
        [this](const sf_pkg_header_t &header_t, const byte_array &data_t) {
            __back_callback(header_t, data_t);
        },
        true);

    sf_bind_signal(
        __tcp_client__, closed, [this]() { close(); }, true);
}

inline void sf_rpc_client::close() const { __tcp_client__->close(); }

inline bool sf_rpc_client::connect_to_server(const std::string ip,
                                             unsigned short port) const {
    return __tcp_client__->connect_to_server(ip, port);
}

inline void sf_rpc_client::__back_callback(const sf_pkg_header_t &header_t,
                                           const byte_array &data_t) {
    if (header_t.type != rpc_res_type) {
        return;
    }
    sf_rpc_res_context_t res;
    from_json(sf_json::from_string(to_string(data_t)), res);
    const auto call_id = res.call_id;
    if (__rpc_data__.count(call_id) == 0) {
        return;
    }
    sf_debug("call ret", to_string(data_t));
    if (__rpc_data__[call_id]->is_async) {
        __rpc_data__[call_id]->async_callback(data_t);
        __rpc_data__.erase(call_id);
    } else {
        __rpc_data__[call_id]->header = header_t;
        __rpc_data__[call_id]->data = data_t;
        __rpc_data__[call_id]->back_finished = true;
        __rpc_data__[call_id]->back_cond.notify_one();
    }
}

inline void sf_rpc_client::__on_closed() {
    for (auto &p : __rpc_data__) {
        if (!p.second->is_async) {
            p.second->back_finished = false;
            p.second->back_cond.notify_one();
        }
    }
}
}    // namespace skyfire

#pragma clang diagnostic pop