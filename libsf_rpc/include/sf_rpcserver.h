/*
 * sf_rpcserver RPC服务器
 */

#pragma once

#include "sf_tcpserver.hpp"
#include "sf_nocopy.hpp"
#include "sf_serialize_binary.hpp"
#include "sf_meta.hpp"
#include <string>
#include <functional>
#include <tuple>
#include <memory>


namespace skyfire {

    class sf_rpcserver : public sf_nocopy<sf_object> {

    SF_REG_SIGNAL(client_connected, SOCKET)
    SF_REG_SIGNAL(client_disconnected, SOCKET)

    private:

        std::shared_ptr<sf_tcpserver> __tcp_server__ = sf_tcpserver::make_server();

        std::vector<std::function<void(SOCKET, int, const std::string &, const byte_array &)>> __func__vec__;

        template<typename _Type>
        void __send_back(SOCKET sock, int id_code, const std::string &id_str, _Type data);


        void __on_data_coming(SOCKET sock, const pkg_header_t &header, const byte_array &data);

        /**
         * @brief sf_rpcserver 构造函数
         */
        sf_rpcserver();
    public:

        /**
         * @brief reg_rpc_func 注册远程调用函数
         * @param id 标识
         * @param func 函数
         */
        template<typename _Func>
        void reg_rpc_func(const std::string &id, _Func func);

        /**
         * @brief make_server 创建RPC服务器
         * @return 服务器对象
         */
        static std::shared_ptr<sf_rpcserver> make_server();



        /**
         * @brief listen 监听
         * @param ip ip
         * @param port 端口
         * @return 是否成功
         */
        bool listen(const std::string &ip, unsigned short port);

        /**
         * @brief close 关闭RPC服务器
         */
        void close();

        friend std::shared_ptr<sf_rpcserver>;

    };

}
