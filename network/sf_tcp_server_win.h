
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_tcp_server_win.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#include <winsock2.h>
#include "core/sf_object.hpp"
#include "core/sf_stdc++.h"
#include "core/sf_type.hpp"
#include "sf_tcp_server_interface.h"
#include "sf_tcp_utils.hpp"
#include "tools/sf_json.hpp"
#include "tools/sf_nocopy.h"

namespace skyfire {
struct sf_per_io_operation_data_t {
    OVERLAPPED overlapped{};
    WSABUF wsa_buffer{};
    byte_array buffer;
    DWORD data_trans_count{};
    bool is_send{};
};

struct sf_per_handle_data_t {
    SOCKET socket;
    byte_array sock_data_buffer;
};

struct sf_iocp_thread_countext_t {
    std::shared_ptr<std::shared_mutex> mu_socks =
        std::make_shared<std::shared_mutex>();
    std::unordered_set<SOCKET> socks;
    HANDLE iocp_port{};
};

class sf_tcp_server
    : public sf_make_instance_t<sf_tcp_server, sf_tcp_server_interface> {
   private:
    bool inited__ = false;
    bool exit_flag__ = false;
    bool raw__ = false;

    SOCKET listen_sock__ = INVALID_SOCKET;

    int thread_count__ = std::thread::hardware_concurrency() * 2 + 2;

    std::vector<std::thread> thread_vec__;

    std::vector<std::unordered_set<SOCKET>> sock_thread__;

    static sf_per_io_operation_data_t* make_req__();

    std::vector<sf_iocp_thread_countext_t> iocp_context__;

    void server_work_thread__(int index);

    bool add_sock__(SOCKET accept_socket);

    void accept_thread__();

    void handle_sock_error__(sf_per_handle_data_t* p_handle_data,
                             sf_per_io_operation_data_t* p_io_data, int index);
    void write_handle__(DWORD bytes_transferred,
                        sf_per_handle_data_t* p_handle_data,
                        sf_per_io_operation_data_t* p_io_data, int index);
    void receive_handle__(DWORD bytes_transferred,
                          sf_per_handle_data_t* p_handle_data,
                          sf_per_io_operation_data_t* p_io_data, int index);

   public:
    SOCKET raw_socket() override;

    explicit sf_tcp_server(bool raw = false);

    ~sf_tcp_server() override;

    bool listen(const std::string& ip, unsigned short port) override;

    void close() override;

    void close(SOCKET sock) override;

    bool send(SOCKET sock, int type, const byte_array& data) override;

    bool send(SOCKET sock, const byte_array& data) override;

    bool detach(SOCKET sock) override;
};

}    // namespace skyfire
