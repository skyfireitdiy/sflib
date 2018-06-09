#pragma once

#include "sf_type.h"
#include "sf_serialize.h"
#include "sf_netutils.h"

namespace skyfire{



    // nat穿透连接上下文
    struct sf_tcp_nat_traversal_context_t__{
        int connect_id;
        unsigned long long src_id;
        addr_info_t src_addr;
        unsigned long long dest_id;
        addr_info_t dest_addr;
        int error_code;
    };
    SF_MAKE_SERIALIZABLE(sf_tcp_nat_traversal_context_t__, connect_id, src_id, src_addr, dest_id, dest_addr, error_code)

}

