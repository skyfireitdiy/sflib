
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

#include "sf_rpc_client.h"
#include "sf_rpc_utils.h"

namespace skyfire {

    int sf_rpc_client::__make_call_id() {
        current_call_id__ = (current_call_id__ + 1) & RPC_REQ_TYPE;
        return current_call_id__;
    }

    template<typename _Ret, typename... __SF_RPC_ARGS__>
    void sf_rpc_client::async_call(const std::string &func_id, std::function<void(_Ret)> rpc_callback,
                                              __SF_RPC_ARGS__... args) {
        static_assert(!std::is_reference<_Ret>::value, "Param can't be reference");
        static_assert(!std::is_pointer<_Ret>::value, "Param can't be pointer");
        static_assert(!std::disjunction<std::is_reference<__SF_RPC_ARGS__>...>::value, "Param can't be reference");
        static_assert(!std::disjunction<std::is_pointer<__SF_RPC_ARGS__>...>::value, "Param can't be pointer");

        using __Ret = typename std::decay<_Ret>::type;
        std::tuple<typename std::decay<__SF_RPC_ARGS__>::type ...> param{args...};
        int call_id = __make_call_id();
        __rpc_data__[call_id] = std::make_shared<sf_rpc_context_t>();
        __rpc_data__[call_id]->is_async = true;
        __rpc_data__[call_id]->async_callback = [=](const byte_array &data) {
            __Ret ret;
            sf_rpc_res_context_t res;
            sf_deserialize_binary(data, res, 0);
            sf_deserialize_binary(res.ret, ret, 0);
            rpc_callback(ret);
        };
        sf_rpc_req_context_t req;
        req.call_id = call_id;
        req.func_id = func_id;
        req.params = sf_serialize_binary(param);
        __tcp_client__->send(RPC_REQ_TYPE, sf_serialize_binary(req));
        auto ptimer = std::make_shared<sf_timer>();
        sf_bind_signal(ptimer, timeout, [=]() {
            __rpc_data__.erase(call_id);
        }, true);
        ptimer->start(rpc_timeout__, true);
    }

    template<typename... __SF_RPC_ARGS__>
    void sf_rpc_client::async_call(const std::string &func_id, std::function<void()> rpc_callback,
                                              __SF_RPC_ARGS__... args) {
        static_assert(!std::disjunction<std::is_reference<__SF_RPC_ARGS__>...>::value, "Param can't be reference");
        static_assert(!std::disjunction<std::is_pointer<__SF_RPC_ARGS__>...>::value, "Param can't be pointer");
        std::tuple<typename std::decay<__SF_RPC_ARGS__>::type ...> param{args...};
        int call_id = __make_call_id();
        __rpc_data__[call_id] = std::make_shared<sf_rpc_context_t>();
        __rpc_data__[call_id]->is_async = true;
        __rpc_data__[call_id]->async_callback = [=](const byte_array &data) {
            rpc_callback();
        };
        sf_rpc_req_context_t req;
        req.call_id = call_id;
        req.func_id = func_id;
        req.params =sf_serialize_binary(param);
        __tcp_client__->send(RPC_REQ_TYPE, sf_serialize_binary(req));
        auto ptimer = std::make_shared<sf_timer>();
        sf_bind_signal(ptimer, timeout, [=]() {
            __rpc_data__.erase(call_id);
        }, true);
        ptimer->start(rpc_timeout__, true);
    }

    template<typename T>
    void sf_rpc_client::async_call(const std::string &func_id, std::function<void()> rpc_callback) {
        int call_id = __make_call_id();
        __rpc_data__[call_id] = std::make_shared<sf_rpc_context_t>();
        __rpc_data__[call_id]->is_async = true;
        __rpc_data__[call_id]->async_callback = [=](const byte_array &data) {
            rpc_callback();
        };
        sf_rpc_req_context_t req;
        req.call_id = call_id;
        req.func_id = func_id;
        req.params =sf_serialize_binary(byte_array());
        __tcp_client__->send(RPC_REQ_TYPE, sf_serialize_binary(req));
    }

    template<typename _Ret, typename... __SF_RPC_ARGS__>
    sf_tri_type<_Ret> sf_rpc_client::call(const std::string &func_id, __SF_RPC_ARGS__... args) {
        static_assert(!std::is_reference<_Ret>::value, "Param can't be reference");
        static_assert(!std::is_pointer<_Ret>::value, "Param can't be pointer");
        static_assert(!std::disjunction<std::is_reference<__SF_RPC_ARGS__>...>::value, "Param can't be reference");
        static_assert(!std::disjunction<std::is_pointer<__SF_RPC_ARGS__>...>::value, "Param can't be pointer");

        using __Ret = typename std::decay<_Ret>::type;

        std::tuple<typename std::decay<__SF_RPC_ARGS__>::type...> param{args...};
        int call_id = __make_call_id();
        __rpc_data__[call_id] = std::make_shared<sf_rpc_context_t>();
        __rpc_data__[call_id]->is_async = false;

        sf_rpc_req_context_t req;
        req.call_id = call_id;
        req.func_id = func_id;
        req.params =sf_serialize_binary(param);
        __tcp_client__->send(RPC_REQ_TYPE, sf_serialize_binary(req));
        std::cout<<"1"<<std::endl;
        if (!__rpc_data__[call_id]->back_finished) {
            std::cout<<"2"<<std::endl;
            std::unique_lock<std::mutex> lck(__rpc_data__[call_id]->back_mu);
            std::cout<<"3"<<std::endl;
            if (__rpc_data__[call_id]->back_cond.wait_for(lck, std::chrono::milliseconds(rpc_timeout__)) ==
                std::cv_status::timeout) {
                std::cout<<"4"<<std::endl;
                __rpc_data__.erase(call_id);
                std::cout<<"5"<<std::endl;
                return sf_tri_type<__Ret>();
            }
            std::cout<<"6"<<std::endl;
        }
        // 连接断开
        if (!__rpc_data__[call_id]->back_finished) {
            return sf_tri_type<__Ret>();
        }
        if constexpr (std::is_same<_Ret, void>::value) {
                std::cout<<"ok"<<std::endl;
            __rpc_data__.erase(call_id);
            return sf_tri_type<void>(true);
        } else {
            sf_tri_type<__Ret> ret;
            __Ret tmp_ret;
            sf_rpc_res_context_t res;
            sf_deserialize_binary(__rpc_data__[call_id]->data, res, 0);
            sf_deserialize_binary(res.ret, tmp_ret, 0);
            ret = tmp_ret;
            __rpc_data__.erase(call_id);
            return ret;
        }
    }

    void sf_rpc_client::set_rpc_timeout(unsigned int ms) {
        rpc_timeout__ = ms;
    }

    sf_rpc_client::sf_rpc_client() {
        sf_bind_signal(__tcp_client__,
                       data_coming,
                       [=](const sf_pkg_header_t &header_t, const byte_array &data_t) {
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


    void sf_rpc_client::close() {
        __tcp_client__->close();
    }


    bool sf_rpc_client::connect_to_server(const std::string ip, unsigned short port) {
        return __tcp_client__->connect_to_server(ip, port);
    }

    std::shared_ptr<sf_rpc_client> sf_rpc_client::make_client() {
        return std::shared_ptr<sf_rpc_client>(new sf_rpc_client);
    }

    
    void sf_rpc_client::__back_callback(const sf_pkg_header_t &header_t, const byte_array &data_t) {
        if(header_t.type != RPC_RES_TYPE)
        {
            return;
        }
        sf_rpc_res_context_t res;
        sf_deserialize_binary(data_t, res, 0);
        int call_id = res.call_id;
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

    void sf_rpc_client::__on_closed() {
        for (auto &p : __rpc_data__) {
            if (!p.second->is_async) {
                p.second->back_finished = false;
                p.second->back_cond.notify_one();
            }
        }
    }
}
