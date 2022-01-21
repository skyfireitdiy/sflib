#include "sflib/rpc_client.h"
#include "sflib/json.h"
#include "sflib/rpc_utils.h"

namespace skyfire
{
int rpc_client::make_call_id__()
{
    current_call_id__ = (current_call_id__ + 1) & rpc_req_type;
    return current_call_id__;
}
void rpc_client::set_rpc_timeout(unsigned int ms)
{
    rpc_timeout__ = ms;
}
rpc_client::rpc_client()
{
    sf_bind(
        tcp_client__, data_coming,
        [this](const pkg_header_t& header_t, const byte_array& data_t) {
            back_callback__(header_t, data_t);
        },
        true);
    sf_bind(
        tcp_client__, closed, [this]() { close(); }, true);
}
void rpc_client::close() const { tcp_client__->close(); }
bool rpc_client::connect_to_server(const std::string ip,
                                   unsigned short    port) const
{
    return tcp_client__->connect_to_server(ip, port);
}
void rpc_client::back_callback__(const pkg_header_t& header_t,
                                 const byte_array&   data_t)
{
    if (header_t.type != rpc_res_type)
    {
        return;
    }
    rpc_res_context_t res;
    from_json(json::from_string(to_string(data_t)), res);
    const auto call_id = res.call_id;
    if (rpc_data__.count(call_id) == 0)
    {
        return;
    }

    if (rpc_data__[call_id]->is_async)
    {
        rpc_data__[call_id]->async_callback(data_t);
        rpc_data__.erase(call_id);
    }
    else
    {
        rpc_data__[call_id]->header        = header_t;
        rpc_data__[call_id]->data          = data_t;
        rpc_data__[call_id]->back_finished = true;
        rpc_data__[call_id]->back_cond.notify_one();
    }
}
void rpc_client::on_closed__()
{
    for (auto& p : rpc_data__)
    {
        if (!p.second->is_async)
        {
            p.second->back_finished = false;
            p.second->back_cond.notify_one();
        }
    }
}
} // namespace skyfire
