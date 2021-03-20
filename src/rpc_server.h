
#pragma once
#include "nocopy.h"
#include "rpc_utils.h"
#include "tcp_server.h"
#include <functional>
#include <memory>
#include <string>
#include <tuple>
namespace skyfire
{
class rpc_server
    : public make_instance_t<rpc_server, nocopy<object>>
{
    sf_singal(client_connected, SOCKET)
        sf_singal(client_disconnected, SOCKET) private : std::unordered_set<SOCKET> client_list__;
    std::shared_mutex           mu_client_list__;
    std::shared_ptr<tcp_server> tcp_server__ = tcp_server::make_instance();
    std::vector<std::function<void(SOCKET, const rpc_req_context_t&)>>
        func_vec__;
    template <typename _Type>
    void send_back__(SOCKET sock, int id_code, _Type data);
    void on_data_coming__(SOCKET sock, const pkg_header_t& header,
                          const byte_array& data);

public:
    rpc_server();
    template <typename _Func>
    void reg_rpc_func(const std::string& id, _Func func);
    bool listen(const std::string& ip, unsigned short port) const;
    void close() const;
    friend std::shared_ptr<rpc_server>;
};
} // namespace skyfire
