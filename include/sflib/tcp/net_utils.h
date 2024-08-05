
#pragma once
#include "sflib/core/type.h"
#include "sflib/io/data_buffer.h"
#include "sflib/json/json.h"
#include "sflib/object/object.h"
#include "sflib/tools/error.h"
#include "sflib/tools/multi_value.h"
#include "sflib/tools/utils.h"
#include <memory>

namespace skyfire
{
struct addr_info_t
{
    std::string    ip;   // ip
    unsigned short port; // 端口
};
SF_JSONIFY(addr_info_t, ip, port)
bool peer_addr(SOCKET sock, addr_info_t& addr);
bool local_addr(SOCKET sock, addr_info_t& addr);
using ip_list_result = multi_value<sf_error, std::vector<std::string>>;
ip_list_result resolve_dns(const std::string& host);
}