
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_tcp_client_linux.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "core/sf_stdc++.h"

#include "sf_tcp_client_interface.h"
#include "tools/sf_utils.h"

namespace skyfire {
class sf_tcp_client
    : public sf_make_instance_t<sf_tcp_client, sf_tcp_client_interface> {
   private:
    bool inited__ = false;
    bool raw__ = false;
    int sock__ = -1;

    void recv_thread__();

   public:
    sf_tcp_client(bool raw = false);    // NOLINT(google-explicit-constructor)

    sf_tcp_client(SOCKET sock,
                  bool raw = false);    // NOLINT(google-explicit-constructor)

    SOCKET raw_socket() override;

    bool bind(const std::string &ip, unsigned short port) override;

    ~sf_tcp_client();    // NOLINT(modernize-use-override)

    bool connect_to_server(const std::string &ip, unsigned short port) override;

    bool send(int type, const byte_array &data) override;

    bool send(const byte_array &data) override;

    void close() override;
};

}    // namespace skyfire
