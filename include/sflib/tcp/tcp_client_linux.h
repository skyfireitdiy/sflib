
#pragma once

#include "sflib/tcp/tcp_client_interface.h"
#include "sflib/tools/error.h"
#include "sflib/tools/utils.h"

#include <arpa/inet.h>
#include <errno.h>
#include <future>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


namespace skyfire
{
class tcp_client
    : public make_instance_t<tcp_client, tcp_client_interface>
{
private:
    bool              inited__ = false;
    bool              raw__    = false;
    int               sock__   = -1;
    std::future<void> loop_thread__;
    void              recv_routine__();

public:
    tcp_client(bool raw = false); // NOLINT(google-explicit-constructor)
    tcp_client(SOCKET sock,
               bool   raw = false); // NOLINT(google-explicit-constructor)
    SOCKET raw_socket() override;
    bool   bind(const std::string& ip, unsigned short port) override;
    ~tcp_client(); // NOLINT(modernize-use-override)
    sf_error connect_to_server(const std::string& host, unsigned short port) override;
    bool     send(int type, const byte_array& data) override;
    bool     send(const byte_array& data) override;
    void     close() override;
};
} // namespace skyfire
