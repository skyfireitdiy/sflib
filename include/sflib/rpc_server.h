
#pragma once
#include "meta.h"
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

template <typename _Type>
void rpc_server::send_back__(const SOCKET sock, const int id_code,
                             _Type data)
{
    rpc_res_context_t res;
    res.call_id = id_code;
    res.ret     = skyfire::to_json(data);

    tcp_server__->send(sock, rpc_res_type,
                       to_byte_array(skyfire::to_json(res).to_string()));
}

template <typename _Func>
void rpc_server::reg_rpc_func(const std::string& id, _Func func)
{
    static_assert(
        !check_param_reference<decltype(std::function(func))>::value,
        "Param can't be reference");
    static_assert(!check_param_pointer<decltype(std::function(func))>::value,
                  "Param can't be pointer");
    static_assert(
        !check_return_reference<decltype(std::function(func))>::value,
        "Return can't be reference");
    static_assert(
        !check_return_pointer<decltype(std::function(func))>::value,
        "Return can't be pointer");
    using _Ret   = typename function_type_helper<decltype(func)>::return_type;
    using _Param = typename function_type_helper<decltype(func)>::param_type;
    // auto 让 constexpr-if 生效
    auto f = [this, id, func](SOCKET s, const auto& req) {
        if (req.func_id == id)
        {
            _Param param;
            from_json(req.params, param);
            if constexpr (std::is_same<_Ret, void>::value)
            {
                invoke(func, param);
                send_back__(s, req.call_id, '\0');
            }
            else
            {
                _Ret ret = invoke(func, param);
                send_back__(s, req.call_id, ret);
            }
        }
    };
    func_vec__.push_back(f);
}

} // namespace skyfire
