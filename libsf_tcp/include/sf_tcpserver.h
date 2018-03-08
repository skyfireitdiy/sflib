#pragma once

#include <winsock2.h>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <map>
#include "sf_tcputils.h"

namespace skyfire
{
class tcpserver : no_copy
{
private:
    bool inited__ = false;
    SOCKET server_sock__ = INVALID_SOCKET;
public:
    tcpserver();
    static std::shared_ptr<tcpserver> make_server();
    ~tcpserver();
    bool listen(const std::string &ip, 
                unsigned short port, 
                std::function<void(SOCKET)> accept_callback,
                std::function<void(SOCKET, const pkg_header_t&,const bytearray_t&)> receive_callback,
                std::function<void(SOCKET)> close_callback);
    void close();
    bool send(SOCKET sock,int type, const bytearray_t& data);
};

}