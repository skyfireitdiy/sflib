
#pragma once
#include "sf_data_buffer.h"
#include "sf_error.h"
#include "sf_json.hpp"
#include "sf_multi_value.h"
#include "sf_object.h"
#include "sf_type.h"
#include "sf_utils.h"
#include <memory>
namespace skyfire
{
struct addr_info_t
{
    std::string    ip;   // ip
    unsigned short port; // 端口
};
SF_JSONIFY(addr_info_t, ip, port)
inline bool peer_addr(SOCKET sock, addr_info_t& addr);
inline bool local_addr(SOCKET sock, addr_info_t& addr);
using ip_list_result = multi_value<sf_error, std::vector<std::string>>;
inline ip_list_result resolve_dns(const std::string& host);
}