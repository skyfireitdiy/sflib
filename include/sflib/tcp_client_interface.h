
#pragma once
#include "error.h"
#include "net_utils.h"
#include "nocopy.h"
#include "object.h"
#include "tcp_utils.h"
#include "type.h"
#include <string>
namespace skyfire
{
class tcp_client_interface : public nocopy<object>
{
    sf_singal(data_coming, const pkg_header_t&, const byte_array&)
        sf_singal(raw_data_coming, const byte_array&)
            sf_singal(closed)
                sf_singal(write_finished)
                    sf_singal(write_error) private : public : virtual SOCKET raw_socket()
                                                              = 0;
    virtual bool     bind(const std::string& ip, unsigned short port) = 0;
    virtual sf_error connect_to_server(const std::string& ip,
                                       unsigned short     port)
        = 0;
    virtual bool send(int type, const byte_array& data) = 0;
    virtual bool send(const byte_array& data)           = 0;
    virtual void close()                                = 0;
    bool         peer_addr(addr_info_t& addr);
    bool         local_addr(addr_info_t& addr);
};
} // namespace skyfire