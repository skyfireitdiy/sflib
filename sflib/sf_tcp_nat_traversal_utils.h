
#pragma once
#include "sf_json.hpp"
#include "sf_nocopy.h"
#include "sf_object.hpp"
#include "sf_stdc++.h"
#include "sf_tcp_client.h"
#include "sf_tcp_server.h"
#include "sf_type.hpp"
namespace skyfire
{
struct tcp_nat_traversal_context_t__ final
{
    std::string        connect_id {};
    unsigned long long src_id {};
    addr_info_t        src_addr;
    unsigned long long dest_id {};
    addr_info_t        dest_addr;
    int                error_code {};
    int                step {};
    bool               raw {};
};
SF_JSONIFY(tcp_nat_traversal_context_t__, connect_id, src_id, src_addr,
           dest_id, dest_addr, error_code, step, raw)
enum class tcp_nat_traversal_connection_type
{
    type_server_valid,
    type_client_valid
};
class tcp_nat_traversal_client;
class tcp_nat_traversal_connection : public nocopy<object>
{
    sf_singal(data_coming, const pkg_header_t&, const byte_array&)
        sf_singal(raw_data_coming, const byte_array&)
            sf_singal(closed) private : std::shared_ptr<tcp_client> client__;
    std::shared_ptr<tcp_server>       server__;
    SOCKET                            sock__;
    std::string                       connect_id__;
    tcp_nat_traversal_connection_type type__;
    tcp_nat_traversal_connection(std::shared_ptr<tcp_client> client,
                                 std::shared_ptr<tcp_server> server,
                                 int sock, std::string connect_id,
                                 tcp_nat_traversal_connection_type type);

public:
    bool send(int type, const byte_array& data) const;
    bool send(const byte_array& data) const;
    friend tcp_nat_traversal_client;
};
} // namespace skyfire
