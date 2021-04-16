
#pragma once
#include "object.hpp"
#include "tcp_client.h"
#include "utils.h"
namespace skyfire
{
class msg_bus_client
    : public make_instance_t<msg_bus_client, nocopy<object>>
{
    sf_singal(msg_come, std::string, byte_array);

public:
    msg_bus_client();
    ~msg_bus_client() override;
    void               reg_msg_to_bus(const std::string& type) const;
    void               reg_msg_to_bus(const std::vector<std::string>& types) const;
    void               unreg_msg_to_bus(const std::string& type) const;
    void               unreg_msg_to_bus(const std::vector<std::string>& types) const;
    [[nodiscard]] bool connect_to_server(const std::string& ip, unsigned short port) const;
    void               send_msg(const std::string& type, const byte_array& data) const;
    void               close() const;
    bool               local_addr(addr_info_t& addr) const;
    bool               peer_addr(addr_info_t& addr) const;

private:
    std::shared_ptr<tcp_client> p_client__ = tcp_client::make_instance();
    void                        on_reg_data__(const pkg_header_t& header, const byte_array& data);
};
} // namespace skyfire
