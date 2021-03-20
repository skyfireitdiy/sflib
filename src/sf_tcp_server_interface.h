
#pragma once
#include "sf_error.h"
#include "sf_net_utils.h"
#include "sf_nocopy.h"
#include "sf_object.h"
#include "sf_server_socket_filter.h"
#include "sf_stdc++.h"
#include "sf_tcp_utils.h"
#include "sf_type.h"
namespace skyfire
{
class tcp_server_interface : public nocopy<object>
{
    sf_singal(new_connection, SOCKET);
    sf_singal(data_coming, SOCKET, const pkg_header_t&,
              const byte_array&);
    sf_singal(raw_data_coming, SOCKET, const byte_array&);
    sf_singal(closed, SOCKET);
    sf_singal(write_finished, SOCKET);
    sf_singal(write_error, SOCKET);
    sf_singal(ready_read, SOCKET);
    sf_singal(ready_write, SOCKET);

private:
    std::vector<std::shared_ptr<server_socket_filter>> filters__;

protected:
    void after_raw_recv_filter__(SOCKET sock, byte_array& data);
    void after_recv_filter__(SOCKET sock, pkg_header_t& header,
                             byte_array& data);
    void before_raw_send_filter__(SOCKET sock, byte_array& data);
    void before_send_filter__(SOCKET sock, pkg_header_t& header,
                              byte_array& data);
    void new_connection_filter__(SOCKET sock);
    void listen_sock_filter__(SOCKET sock);
    void disconnect_sock_filter__(SOCKET sock);

public:
    void add_server_socket_filter(
        std::shared_ptr<server_socket_filter>&& filter);
    virtual SOCKET   raw_socket()                                        = 0;
    virtual bool     listen(const std::string& ip, unsigned short port)  = 0;
    virtual void     close()                                             = 0;
    virtual void     close(SOCKET sock)                                  = 0;
    virtual bool     send(SOCKET sock, int type, const byte_array& data) = 0;
    virtual bool     send(SOCKET sock, const byte_array& data)           = 0;
    bool             server_addr(addr_info_t& addr);
    virtual sf_error recv(SOCKET sock, byte_array& data, int length = default_buffer_size) = 0;
};
} // namespace skyfire