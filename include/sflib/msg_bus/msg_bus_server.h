
#pragma once
#include "sflib/object/object.h"
#include "sflib/tcp/tcp_server.h"
#include "sflib/tools/utils.h"
#include <string>
namespace skyfire
{
class msg_bus_server final
    : public make_instance_t<msg_bus_server, nocopy<object>>
{
    sf_singal(msg_come, SOCKET, std::string, byte_array);

public:
    msg_bus_server();
    ~msg_bus_server() override;
    bool listen(const std::string& ip, unsigned short port) const;
    void close();
    void clear_client();
    void send_msg(const std::string& type, const byte_array& data);
    bool server_addr(addr_info_t& addr) const;

private:
    std::shared_ptr<tcp_server>                        p_server__ = tcp_server::make_instance();
    std::unordered_map<std::string, std::list<SOCKET>> msg_map__;
    void                                               reg_msg__(SOCKET sock, const std::string& msg_name);
    void                                               on_reg_data__(SOCKET sock, const pkg_header_t& header,
                                                                     const byte_array& data);
    void                                               unreg_msg__(SOCKET sock, const std::string& msg);
    void                                               on_disconnect__(SOCKET sock);
};
} // namespace skyfire
