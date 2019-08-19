
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_tcp_server_linux.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <unistd.h>
#include <memory>
#include <string>
#include "core/sf_object.hpp"
#include "core/sf_type.hpp"
#include "sf_tcp_server_interface.h"
#include "sf_tcp_utils.hpp"
#include "tools/sf_json.hpp"
#include "tools/sf_logger.hpp"
#include "tools/sf_nocopy.h"

#include "tools/sf_utils.h"

namespace skyfire {

struct sock_data_context_t {
    epoll_event ev{};
    byte_array data_buffer_in{};
    std::shared_ptr<std::shared_mutex> mu_data_buffer_out =
        std::make_shared<std::shared_mutex>();
    std::deque<byte_array> data_buffer_out{};
};

struct epoll_context_t {
    int epoll_fd{};
    std::unordered_map<SOCKET, sock_data_context_t> sock_context__{};
};

class sf_tcp_server
    : public sf_make_instance_t<sf_tcp_server, sf_tcp_server_interface> {
   private:
    int listen_fd__ = -1;
    bool raw__ = false;
    int thread_count__ = std::thread::hardware_concurrency() * 2 + 2;

    std::vector<std::thread> thread_vec__;

    void work_thread__(bool listen_thread = false, SOCKET listen_fd = -1);

    bool in_dispatch__(SOCKET fd);

    bool handle_accept__();

    void handle_read__(const epoll_event &ev);

    void handle_write__(const epoll_event &ev);

    epoll_context_t &epoll_data__() const;

   public:
    SOCKET raw_socket() override;

    explicit sf_tcp_server(bool raw = false);

    ~sf_tcp_server() override;

    bool listen(const std::string &ip, unsigned short port) override;

    void close() override;

    void close(SOCKET sock) override;

    bool send(SOCKET sock, int type, const byte_array &data) override;

    bool send(SOCKET sock, const byte_array &data) override;
};

}    // namespace skyfire