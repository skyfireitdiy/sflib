
/**
* @version 1.0.0
* @author skyfire
* @file sf_tcp_server_win.h
*/

#pragma once

#include <winsock2.h>
#include "sf_object.hpp"
#include "sf_stdc++.h"
#include "sf_type.hpp"
#include "sf_tcp_server_interface.h"
#include "sf_tcp_utils.hpp"
#include "sf_json.hpp"
#include "sf_nocopy.h"

namespace skyfire {
    
/**
 * @brief 每个操作数据
 * 
 */
struct per_io_operation_data_t {
    OVERLAPPED overlapped{};
    WSABUF wsa_buffer{};
    byte_array buffer;
    DWORD data_trans_count{};
    bool is_send{};
};

/**
 * @brief socket数据
 * 
 */
struct per_handle_data_t {
    SOCKET socket;
    byte_array sock_data_buffer;
};

/**
 * @brief iocp线程上下文
 * 
 */
struct iocp_thread_countext_t {
    std::shared_ptr<std::shared_mutex> mu_socks =
        std::make_shared<std::shared_mutex>();
    std::unordered_set<SOCKET> socks;
    HANDLE iocp_port{};
};

class tcp_server
    : public make_instance_t<tcp_server, tcp_server_interface> {
   private:
    bool inited__ = false;
    bool exit_flag__ = false;
    bool raw__ = false;

    SOCKET listen_sock__ = INVALID_SOCKET;

    int thread_count__ = std::thread::hardware_concurrency() * 2 + 2;

    std::vector<std::thread> thread_vec__;

    std::vector<std::unordered_set<SOCKET>> sock_thread__;

    static per_io_operation_data_t* make_req__();

    std::vector<iocp_thread_countext_t> iocp_context__;

    void server_work_thread__(int index);

    bool add_sock__(SOCKET accept_socket);

    void accept_thread__();

    void handle_sock_error__(per_handle_data_t* p_handle_data,
                             per_io_operation_data_t* p_io_data, int index);
    void write_handle__(DWORD bytes_transferred,
                        per_handle_data_t* p_handle_data,
                        per_io_operation_data_t* p_io_data, int index);
    void receive_handle__(DWORD bytes_transferred,
                          per_handle_data_t* p_handle_data,
                          per_io_operation_data_t* p_io_data, int index);

   public:
    /**
     * @brief 获取原始套接字
     * 
     * @return SOCKET 原始套接字
     */
    SOCKET raw_socket() override;

    /**
     * @brief 构造一个tcp服务器
     * 
     * @param raw 是否是原始包
     * @param thread_count 线程数量
     */
    explicit tcp_server(bool raw = false, int thread_count = std::thread::hardware_concurrency() * 2 + 2);

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
};

}    // namespace skyfire
