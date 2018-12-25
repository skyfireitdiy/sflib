
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_rpc_server.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * sf_rpc_server RPC服务器
 */

#pragma once

#include "sf_tcp_server.h"
#include "sf_nocopy.h"
#include "sf_rpc_utils.h"
#include <string>
#include <functional>
#include <tuple>
#include <memory>

namespace skyfire {

    /**
     *  @brief rpc server类
     */
    class sf_rpc_server : public sf_nocopy<sf_object> {
        /**
         * 客户端连接信号
         */
    SF_REG_SIGNAL(client_connected, SOCKET)
    /**
     * 客户端断开连接信号
     */
    SF_REG_SIGNAL(client_disconnected, SOCKET)

    private:

        std::shared_ptr<sf_tcp_server> __tcp_server__ = sf_tcp_server::make_server();

        std::vector<std::function<void(SOCKET, const sf_rpc_req_context_t &)>> __func__vec__;

        template<typename _Type>
        void __send_back(SOCKET sock, int id_code, _Type data);


        void __on_data_coming(SOCKET sock, const sf_pkg_header_t &header, const byte_array &data);

        sf_rpc_server();
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
        static std::shared_ptr<sf_rpc_server> make_server();



        /**
         * @brief listen 监听
         * @param ip ip
         * @param port 端口
         * @return 是否成功
         */
        bool listen(const std::string &ip, unsigned short port) const;

        /**
         * @brief close 关闭RPC服务器
         */
        void close() const;

        friend std::shared_ptr<sf_rpc_server>;

    };

}
