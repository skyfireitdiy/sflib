
/**
* @version 1.0.0
* @author skyfire
* @file sf_tcp_server_linux.h
*/

#pragma once

#include "sf_json.hpp"
#include "sf_logger.hpp"
#include "sf_nocopy.h"
#include "sf_object.hpp"
#include "sf_tcp_server_interface.h"
#include "sf_tcp_utils.hpp"
#include "sf_type.hpp"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <memory>
#include <netinet/in.h>
#include <string>
#include <sys/epoll.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <unistd.h>

#include "sf_utils.h"

namespace skyfire
{

/**
 * @brief socket 数据上下文
 * 
 */
struct sock_data_context_t
{
    epoll_event            ev {};
    byte_array             data_buffer_in {};
    std::deque<byte_array> data_buffer_out {};
};

/**
 * @brief epoll上下文
 * 
 */
struct epoll_context_t
{
    int epoll_fd {};

    std::shared_mutex                               mu_epoll_context__;
    std::unordered_map<SOCKET, sock_data_context_t> sock_context__ {};
};

class tcp_server
    : public make_instance_t<tcp_server, tcp_server_interface>
{
private:
    std::atomic<bool> closed__ = false;
    
    tcp_server_opt_t config__;

    SOCKET listen_fd__;

    std::vector<std::thread> thread_vec__;

    std::vector<epoll_context_t*> context_pool__;

    mutable std::shared_mutex mu_context_pool__;

    void work_thread__(bool listen_thread = false, SOCKET listen_fd = -1);

    bool in_dispatch__(SOCKET fd);

    bool handle_accept__();

    void handle_read__(const epoll_event& ev);

    void handle_write__(const epoll_event& ev);

    epoll_context_t& epoll_data__() const;

    epoll_context_t* find_context__(SOCKET sock) const;

public:
    /**
     * @brief 获取原始套接字
     * 
     * @return SOCKET 原始套接字
     */
    SOCKET raw_socket() override;

    template<typename ...Args>
    tcp_server(Args&& ...args);
    

    ~tcp_server() override;

    /**
     * @brief 监听
     * 
     * @param ip ip地址
     * @param port 端口
     * @return true 监听成功
     * @return false 监听失败
     */
    bool listen(const std::string& ip, unsigned short port) override;

    /**
     * @brief 关闭服务器
     * 
     */
    void close() override;

    /**
     * @brief 关闭指定的客户端
     * 
     * @param sock 客户端socket
     */
    void close(SOCKET sock) override;

    /**
     * @brief 发送消息
     * 
     * @param sock 要发送的socket
     * @param type 类型
     * @param data 数据
     * @return true 发送成功
     * @return false 发送失败
     */
    bool send(SOCKET sock, int type, const byte_array& data) override;

    /**
     * @brief 发送数据（原始）
     * 
     * @param sock 要发送的socket
     * @param data 数据
     * @return true 发送成功
     * @return false 发送失败
     */
    bool send(SOCKET sock, const byte_array& data) override;

    sf_error recv(SOCKET sock, byte_array& data, int length = default_buffer_size) override;
};

} // namespace skyfire