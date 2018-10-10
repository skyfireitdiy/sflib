/*
 * sf_rpcclient rpc客户端
 */

#pragma once

#include "sf_rpcclient.h"

namespace skyfire {

    int sf_rpcclient::__make_call_id() {
        current_call_id__ = (current_call_id__ + 1) & TCP_RPC_TYPE_MASK;
        return current_call_id__;
    }

    template<typename _Ret, typename... __SF_RPC_ARGS__>
    void sf_rpcclient::async_call(const std::string &func_id, std::function<void(_Ret)> rpc_callback,
                                              __SF_RPC_ARGS__... args) {
        static_assert(!std::is_reference<_Ret>::value, "Param can't be reference");
        static_assert(!std::is_pointer<_Ret>::value, "Param can't be pointer");
        static_assert(!std::disjunction<std::is_reference<__SF_RPC_ARGS__>...>::value, "Param can't be reference");
        static_assert(!std::disjunction<std::is_pointer<__SF_RPC_ARGS__>...>::value, "Param can't be pointer");

        using __Ret = typename std::decay<_Ret>::type;
        std::tuple<typename std::decay<__SF_RPC_ARGS__>::type ...> param{args...};
        int call_id = __make_call_id();
        __rpc_data__[call_id] = std::make_shared<rpc_struct>();
        __rpc_data__[call_id]->is_async = true;
        __rpc_data__[call_id]->async_callback = [=](const byte_array &data) {
            __Ret ret;
            std::string id_str;
            size_t pos = sf_deserialize_binary(data, id_str, 0);
            sf_deserialize_binary(data, ret, pos);
            rpc_callback(ret);
        };
        __tcp_client__->send(call_id, sf_serialize_binary(std::string(func_id)) + sf_serialize_binary(param));
        auto ptimer = std::make_shared<sf_timer>();
        sf_bind_signal(ptimer, timeout, [=]() {
            __rpc_data__.erase(call_id);
        }, true);
        ptimer->start(rpc_timeout__, true);
    }

    template<typename... __SF_RPC_ARGS__>
    void sf_rpcclient::async_call(const std::string &func_id, std::function<void()> rpc_callback,
                                              __SF_RPC_ARGS__... args) {
        static_assert(!std::disjunction<std::is_reference<__SF_RPC_ARGS__>...>::value, "Param can't be reference");
        static_assert(!std::disjunction<std::is_pointer<__SF_RPC_ARGS__>...>::value, "Param can't be pointer");
        std::tuple<typename std::decay<__SF_RPC_ARGS__>::type ...> param{args...};
        int call_id = __make_call_id();
        __rpc_data__[call_id] = std::make_shared<rpc_struct>();
        __rpc_data__[call_id]->is_async = true;
        __rpc_data__[call_id]->async_callback = [=](const byte_array &data) {
            rpc_callback();
        };
        __tcp_client__->send(call_id, sf_serialize_binary(std::string(func_id)) + sf_serialize_binary(param));
        auto ptimer = std::make_shared<sf_timer>();
        sf_bind_signal(ptimer, timeout, [=]() {
            __rpc_data__.erase(call_id);
        }, true);
        ptimer->start(rpc_timeout__, true);
    }

    template<typename T>
    void sf_rpcclient::async_call(const std::string &func_id, std::function<void()> rpc_callback) {
        int call_id = __make_call_id();
        __rpc_data__[call_id] = std::make_shared<rpc_struct>();
        __rpc_data__[call_id]->is_async = true;
        __rpc_data__[call_id]->async_callback = [=](const byte_array &data) {
            rpc_callback();
        };
        __tcp_client__->send(call_id, sf_serialize_binary(std::string(func_id)));
    }

    template<typename _Ret, typename... __SF_RPC_ARGS__>
    sf_tri_type<_Ret> sf_rpcclient::call(const std::string &func_id, __SF_RPC_ARGS__... args) {
        static_assert(!std::is_reference<_Ret>::value, "Param can't be reference");
        static_assert(!std::is_pointer<_Ret>::value, "Param can't be pointer");
        static_assert(!std::disjunction<std::is_reference<__SF_RPC_ARGS__>...>::value, "Param can't be reference");
        static_assert(!std::disjunction<std::is_pointer<__SF_RPC_ARGS__>...>::value, "Param can't be pointer");

        using __Ret = typename std::decay<_Ret>::type;

        std::tuple<typename std::decay<__SF_RPC_ARGS__>::type...> param{args...};
        int call_id = __make_call_id();
        __rpc_data__[call_id] = std::make_shared<rpc_struct>();
        __rpc_data__[call_id]->is_async = false;
        __tcp_client__->send(call_id, sf_serialize_binary(std::string(func_id)) + sf_serialize_binary(param));
        if (!__rpc_data__[call_id]->back_finished) {
            std::unique_lock<std::mutex> lck(__rpc_data__[call_id]->back_mu);
            if (__rpc_data__[call_id]->back_cond.wait_for(lck, std::chrono::milliseconds(rpc_timeout__)) ==
                std::cv_status::timeout) {
                __rpc_data__.erase(call_id);
                return sf_tri_type<__Ret>();
            }
        }

        // 连接断开
        if (!__rpc_data__[call_id]->back_finished) {
            return sf_tri_type<__Ret>();
        }

        std::string id_str;
        if constexpr (std::is_same<_Ret, void>::value) {
            __rpc_data__.erase(call_id);
            return sf_tri_type<void>(true);
        } else {
            sf_tri_type<__Ret> ret;
            __Ret tmp_ret;
            size_t pos = sf_deserialize_binary(__rpc_data__[call_id]->data, id_str, 0);
            sf_deserialize_binary(__rpc_data__[call_id]->data, tmp_ret, pos);
            ret = tmp_ret;
            __rpc_data__.erase(call_id);
            return ret;
        }
    }

    void sf_rpcclient::set_rpc_timeout(unsigned int ms) {
        rpc_timeout__ = ms;
    }

    sf_rpcclient::sf_rpcclient() {
        sf_bind_signal(__tcp_client__,
                       data_coming,
                       [=](const pkg_header_t &header_t, const byte_array &data_t) {
                           __back_callback(header_t, data_t);
                       },
                       true);


        sf_bind_signal(__tcp_client__,
                       closed,
                       [=]() {
                           close();
                       },
                       true);
    }


    void sf_rpcclient::close() {
        __tcp_client__->close();
    }


    bool sf_rpcclient::connect(const std::string ip, unsigned short port) {
        return __tcp_client__->connect(ip, port);
    }

    std::shared_ptr<sf_rpcclient> sf_rpcclient::make_client() {
        return std::shared_ptr<sf_rpcclient>(new sf_rpcclient);
    }

    
    void sf_rpcclient::__back_callback(const pkg_header_t &header_t, const byte_array &data_t) {
        int call_id = header_t.type;
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

    void sf_rpcclient::__on_closed() {
        for (auto &p : __rpc_data__) {
            if (!p.second->is_async) {
                p.second->back_finished = false;
                p.second->back_cond.notify_one();
            }
        }
    }
}
