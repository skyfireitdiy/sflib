/*
 * sf_rpcserver RPC服务器
 */

#pragma once

#include "sf_rpcserver.h"

namespace skyfire {

    template<typename _BaseClass>
    template<typename _Type>
    void sf_rpcserver<_BaseClass>::__send_back(SOCKET sock, int id_code, const std::string &id_str, _Type data) {
        __tcp_server__->send(sock, id_code, sf_serialize_binary(id_str) + sf_serialize_binary(data));
    }

    template<typename _BaseClass>
    void sf_rpcserver<_BaseClass>::__on_data_coming(SOCKET sock, const pkg_header_t &header, const byte_array &data) {
        std::string id;
        byte_array param;
        size_t pos = sf_deserialize_binary(data, id, 0);
        for (auto &p : __func__vec__) {
            p(sock, header.type, id, byte_array(data.begin() + pos, data.end()));
        }
    }

    template<typename _BaseClass>
    template<typename _Func>
    void sf_rpcserver<_BaseClass>::reg_rpc_func(const std::string &id, _Func func) {
        if constexpr (std::is_bind_expression<_Func>::value) {
            static_assert(!sf_check_param_reference<_Func>::value, "Param can't be reference");
            static_assert(!sf_check_param_pointer<_Func>::value, "Param can't be pointer");
            static_assert(!sf_check_return_reference<_Func>::value, "Return can't be reference");
            static_assert(!sf_check_return_pointer<_Func>::value, "Return can't be pointer");


            using _Ret = typename sf_function_type_helper<_Func>::return_type;
            using _Param = typename sf_function_type_helper<_Func>::param_type;

            auto f = [=](SOCKET s, int id_code, const std::string &id_str, const byte_array &data) {
                if (id == id_str) {
                    _Param param;
                    sf_deserialize_binary(data, param, 0);
                    _Ret ret = sf_invoke(func, param);
                    __send_back(s, id_code, id_str, ret);
                }
            };
            __func__vec__.push_back(f);

        } else {
            static_assert(!sf_check_param_reference<decltype(std::function(func))>::value,
                          "Param can't be reference");
            static_assert(!sf_check_param_pointer<decltype(std::function(func))>::value, "Param can't be pointer");
            static_assert(!sf_check_return_reference<decltype(std::function(func))>::value,
                          "Return can't be reference");
            static_assert(!sf_check_return_pointer<decltype(std::function(func))>::value,
                          "Return can't be pointer");


            using _Ret = typename sf_function_type_helper<decltype(std::function(func))>::return_type;
            using _Param = typename sf_function_type_helper<decltype(std::function(func))>::param_type;

            auto f = [=](SOCKET s, int id_code, const std::string &id_str, const byte_array &data) {
                if (id == id_str) {
                    _Param param;
                    sf_deserialize_binary(data, param, 0);
                    if constexpr (std::is_same<_Ret, void>::value) {
                        sf_invoke(func, param);
                        __send_back(s, id_code, id_str, '\0');
                    } else {
                        _Ret ret = sf_invoke(func, param);
                        __send_back(s, id_code, id_str, ret);
                    }
                }
            };
            __func__vec__.push_back(f);
        }

    }

    template<typename _BaseClass>
    std::shared_ptr<sf_rpcserver<_BaseClass>> sf_rpcserver<_BaseClass>::make_server() {
        return std::make_shared<sf_rpcserver<_BaseClass>>();
    }

    template<typename _BaseClass>
    sf_rpcserver<_BaseClass>::sf_rpcserver() {
        sf_bind_signal(sf_rpcserver::__tcp_server__,
                       data_coming,
                       [=](SOCKET sock, const pkg_header_t &header, const byte_array &data) {
                           __on_data_coming(sock, header, data);
                       },
                       true);
    }

    template<typename _BaseClass>
    bool sf_rpcserver<_BaseClass>::listen(const std::string &ip, unsigned short port) {
        return __tcp_server__->listen(ip, port);
    }

    template<typename _BaseClass>
    void sf_rpcserver<_BaseClass>::close() {
        __tcp_server__->close();
    }
}
