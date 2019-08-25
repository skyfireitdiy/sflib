
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_rpc_server.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * sf_rpc_server RPC服务器
 */

#pragma once

#include "sf_rpc_server.h"
#include "sf_rpc_utils.h"

namespace skyfire {
template <typename _Type>
void sf_rpc_server::__send_back(const SOCKET sock, const int id_code,
                                _Type data) {
    sf_rpc_res_context_t res;
    res.call_id = id_code;
    res.ret = skyfire::to_json(data);
    sf_debug("call ret", skyfire::to_json(res));
    __tcp_server__->send(sock, rpc_res_type,
                         to_byte_array(skyfire::to_json(res).to_string()));
}

inline void sf_rpc_server::__on_data_coming(const SOCKET sock,
                                            const sf_pkg_header_t &header,
                                            const byte_array &data) {
    if (header.type != rpc_req_type) {
        return;
    }
    std::string id;
    byte_array param;
    sf_rpc_req_context_t req;
    sf_debug("call req", to_string(data));
    from_json(sf_json::from_string(to_string(data)), req);
    for (auto &p : __func__vec__) {
        p(sock, req);
    }
}

template <typename _Func>
void sf_rpc_server::reg_rpc_func(const std::string &id, _Func func) {
    static_assert(
        !sf_check_param_reference<decltype(std::function(func))>::value,
        "Param can't be reference");
    static_assert(!sf_check_param_pointer<decltype(std::function(func))>::value,
                  "Param can't be pointer");
    static_assert(
        !sf_check_return_reference<decltype(std::function(func))>::value,
        "Return can't be reference");
    static_assert(
        !sf_check_return_pointer<decltype(std::function(func))>::value,
        "Return can't be pointer");

    using _Ret = typename sf_function_type_helper<decltype(func)>::return_type;
    using _Param = typename sf_function_type_helper<decltype(func)>::param_type;
    // auto 让 constexpr-if 生效
    auto f = [this, id, func](SOCKET s, const auto &req) {
        if (req.func_id == id) {
            _Param param;
            from_json(req.params, param);
            if constexpr (std::is_same<_Ret, void>::value) {
                sf_invoke(func, param);
                __send_back(s, req.call_id, '\0');
            } else {
                _Ret ret = sf_invoke(func, param);
                __send_back(s, req.call_id, ret);
            }
        }
    };
    __func__vec__.push_back(f);
}

inline sf_rpc_server::sf_rpc_server() {
    sf_bind_signal(
        sf_rpc_server::__tcp_server__, data_coming,
        [this](SOCKET sock, const sf_pkg_header_t &header,
               const byte_array &data) {
            __on_data_coming(sock, header, data);
        },
        true);
    sf_bind_signal(
        sf_rpc_server::__tcp_server__, new_connection,
        [this](SOCKET sock) { client_connected(sock); }, true);
    sf_bind_signal(
        sf_rpc_server::__tcp_server__, closed,
        [this](SOCKET sock) { client_disconnected(sock); }, true);
}

inline bool sf_rpc_server::listen(const std::string &ip,
                                  unsigned short port) const {
    return __tcp_server__->listen(ip, port);
}

inline void sf_rpc_server::close() const { __tcp_server__->close(); }
}    // namespace skyfire
