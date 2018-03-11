#pragma once

#include "sf_tcpserver.h"
#include "sf_nocopy.h"
#include "sf_serialize.h"
#include <string>
#include <functional>
#include <tuple>
#include <memory>

#include <iostream>

#ifdef SF_RPC_CLIENT
#error You cannot include 'sf_rpcserver.h' and 'sf_rpcclient.h' at this same time
#endif

#define SF_RPC_SERVER
#include "sf_rpcutils.h"

namespace skyfire
{

    template <typename _BaseClass = sf_empty_class>
    class sf_rpcserver : public sf_nocopy<_BaseClass>
    {
    protected:

        std::shared_ptr<sf_tcpserver> __tcp_server__ = sf_tcpserver::make_server();

        template<typename Function, typename Tuple, std::size_t... Index>
        decltype(auto) __invoke_impl(Function&& func, Tuple&& t, std::index_sequence<Index...>)
        {
            return func(std::get<Index>(std::forward<Tuple>(t))...);
        }

        template<typename Function, typename Tuple>
        decltype(auto) __invoke(Function&& func, Tuple&& t)
        {
            constexpr auto size = std::tuple_size<typename std::decay<Tuple>::type>::value;
            return __invoke_impl(std::forward<Function>(func), std::forward<Tuple>(t), std::make_index_sequence<size>{});
        }

        template <typename _Type>
        void __send_back(SOCKET sock, int id_code, const std::string& id_str, _Type data)
        {
            std::cout<<"return id:"<<id_str<<" value:"<<data<<std::endl;
            __tcp_server__->send(sock, id_code, sf_serialize(id_str)+sf_serialize(data));
        }


        void __on_data_coming(SOCKET sock,const pkg_header_t& header,const byte_array& data)
        {
            std::string id;
            byte_array param;
            size_t pos = sf_deserialize(data, id, 0);
            std::cout<<"called:"<<id<<std::endl;
            __rpc_callback__(sock, header.type, id, byte_array(data.begin() + pos, data.end()));
        }

    public:

        virtual void __rpc_callback__(SOCKET s, int id_code, const std::string &id_str,const byte_array& data) = 0;

        sf_rpcserver()
        {
            sf_bind_signal(__tcp_server__,
                           data_coming,
                           std::bind(__on_data_coming,
                                     this,
                                     std::placeholders::_1,
                                     std::placeholders::_2,
                                     std::placeholders::_3),
                           false);
        }

        bool listen(const std::string& ip, unsigned short port)
        {
            __tcp_server__->listen(ip,port);
        }

        void close()
        {
            __tcp_server__->close();
        }

    };
}