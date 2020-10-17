
/**
* @version 1.0.0
* @author skyfire
* @file sf_rpc_server.h
*/

/*
 * rpc_server RPC服务器
 */

#pragma once

#include "sf_rpc_utils.h"
#include "sf_tcp_server.h"
#include "sf_nocopy.h"
#include <functional>
#include <memory>
#include <string>
#include <tuple>

namespace skyfire {

/**
 *  @brief rpc server类
 */
class rpc_server
    : public make_instance_t<rpc_server, nocopy<object>> {
    /**
     * 客户端连接信号
     */
    sf_singal(client_connected, SOCKET)
    /**
     * 客户端断开连接信号
     */
    sf_singal(client_disconnected, SOCKET)

private:
    std::unordered_set<SOCKET> client_list__;

    std::shared_mutex mu_client_list__;

    std::shared_ptr<tcp_server> tcp_server__ = tcp_server::make_instance();

    std::vector<std::function<void(SOCKET, const rpc_req_context_t&)>>
        func_vec__;

    template <typename _Type>
    void send_back__(SOCKET sock, int id_code, _Type data);

    void on_data_coming__(SOCKET sock, const pkg_header_t& header,
        const byte_array& data);

public:
    rpc_server();

    /**
     * @brief reg_rpc_func 注册远程调用函数
     * @param id 标识
     * @param func 函数
     */
    template <typename _Func>
    void reg_rpc_func(const std::string& id, _Func func);

    /**
     * @brief listen 监听
     * @param ip ip
     * @param port 端口
     * @return 是否成功
     */
    bool listen(const std::string& ip, unsigned short port) const;

    /**
     * @brief close 关闭RPC服务器
     */
    void close() const;

    friend std::shared_ptr<rpc_server>;
};

} // namespace skyfire
