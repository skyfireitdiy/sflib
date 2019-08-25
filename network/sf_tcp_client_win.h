
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_tcp_client_win.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#include <winsock2.h>
#include "sf_tcp_client_interface.h"
#include "tools/sf_utils.h"

namespace skyfire {
class sf_tcp_client final
    : public sf_make_instance_t<sf_tcp_client, sf_tcp_client_interface> {
   private:
    bool raw__ = false;
    bool inited__ = false;
    SOCKET sock__ = INVALID_SOCKET;

    void recv_thread__();

   public:
    SOCKET raw_socket() override;

    bool bind(const std::string &ip, unsigned short port) override;

    explicit sf_tcp_client(bool raw = false);

    explicit sf_tcp_client(SOCKET sock, bool raw = false);

    ~sf_tcp_client();

    bool connect_to_server(const std::string &ip, unsigned short port) override;

    bool send(int type, const byte_array &data) override;

    bool send(const byte_array &data) override;

    void close() override;
};

}    // namespace skyfire
