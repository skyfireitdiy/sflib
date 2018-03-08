#pragma once

#include <winsock2.h>
#include <string>
#include <functional>
#include <memory>
#include "sf_tcputils.h"

namespace skyfire
{

class tcpclient : no_copy
{
private:
    bool inited__ = false;
    SOCKET sock__ = INVALID_SOCKET;
public:
    tcpclient();
    static std::shared_ptr<tcpclient> make_client();
    ~tcpclient();
    bool connect(const std::string& ip, 
                unsigned short port, 
                std::function<void(const pkg_header_t &,const bytearray_t&)> receive_callback, 
                std::function<void()> close_callback);
    bool send(int type,const bytearray_t& data);
    void close();
};

}
