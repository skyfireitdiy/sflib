
#include "sflib/rpc/rpc_server.h"
#include "sflib/rpc/rpc_utils.h"
namespace skyfire
{

void rpc_server::on_data_coming__(const SOCKET        sock,
                                  const pkg_header_t& header,
                                  const byte_array&   data)
{
    if (header.type != rpc_req_type)
    {
        return;
    }
    std::string       id;
    byte_array        param;
    rpc_req_context_t req;

    from_json(json::from_string(to_string(data)), req);
    for (auto& p : func_vec__)
    {
        p(sock, req);
    }
}
rpc_server::rpc_server()
{
    sf_bind(
        rpc_server::tcp_server__, data_coming,
        [this](SOCKET sock, const pkg_header_t& header,
               const byte_array& data) {
            on_data_coming__(sock, header, data);
        },
        true);
    sf_bind(
        rpc_server::tcp_server__, new_connection,
        [this](SOCKET sock) {
            {
                std::unique_lock lck(mu_client_list__);
                client_list__.insert(sock);
            }
            client_connected(sock);
        },
        true);
    sf_bind(
        rpc_server::tcp_server__, closed,
        [this](SOCKET sock) {
            {
                std::unique_lock lck(mu_client_list__);
                client_list__.erase(sock);
            }
            client_disconnected(sock);
        },
        true);
}
bool rpc_server::listen(const std::string& ip,
                        unsigned short     port) const
{
    return tcp_server__->listen(ip, port);
}
void rpc_server::close() const { tcp_server__->close(); }
} // namespace skyfire
