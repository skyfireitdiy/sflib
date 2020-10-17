
/**
* @version 1.0.0
* @author skyfire
* @file sf_tcp_client_win.h
*/

#pragma once

#include <winsock2.h>
#include "sf_tcp_client_interface.h"
#include "sf_utils.h"

namespace skyfire {
class tcp_client final
    : public make_instance_t<tcp_client, tcp_client_interface> {
   private:
    bool raw__ = false;
    bool inited__ = false;
    SOCKET sock__ = INVALID_SOCKET;

    void recv_thread__();

   public:
    SOCKET raw_socket() override;

    bool sf_bind(const std::string &ip, unsigned short port) override;

    explicit tcp_client(bool raw = false);

    explicit tcp_client(SOCKET sock, bool raw = false);

    ~tcp_client();

    bool connect_to_server(const std::string &ip, unsigned short port) override;

    bool send(int type, const byte_array &data) override;

    bool send(const byte_array &data) override;

    void close() override;
};

}    // namespace skyfire
