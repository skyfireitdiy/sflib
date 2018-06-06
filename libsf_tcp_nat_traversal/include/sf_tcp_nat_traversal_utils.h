#pragma once

#include "sf_type.h"
#include "sf_serialize.h"

namespace skyfire{

    struct addr_info_t{
        std::string ip;
        unsigned short port;
    };

    struct sf_tcp_nat_traversal_context_t__{
        int connect_id;
        unsigned long long src_id;
        addr_info_t src_addr;
        unsigned long long dest_id;
        addr_info_t dest_addr;
    };



    inline byte_array sf_serialize(const addr_info_t& obj){
        return sf_serialize(obj.ip) + sf_serialize(obj.port);
    }

    inline size_t
    sf_deserialize(const byte_array &data, addr_info_t &obj, size_t begin_pos){
        auto pos = begin_pos;
        pos = sf_deserialize(data, obj.ip, pos);
        pos = sf_deserialize(data, obj.port, pos);
        return pos;
    }


    inline byte_array sf_serialize(const sf_tcp_nat_traversal_context_t__& obj){
        return sf_serialize(obj.connect_id) +
               sf_serialize(obj.src_id) +
               sf_serialize(obj.src_addr) +
               sf_serialize(obj.dest_id) +
               sf_serialize(obj.dest_addr);
    }

    inline size_t
    sf_deserialize(const byte_array &data, sf_tcp_nat_traversal_context_t__ &obj, size_t begin_pos){
        auto pos = begin_pos;
        pos = sf_deserialize(data, obj.connect_id, pos);
        pos = sf_deserialize(data, obj.src_id, pos);
        pos = sf_deserialize(data, obj.src_addr, pos);
        pos = sf_deserialize(data, obj.dest_id, pos);
        pos = sf_deserialize(data, obj.dest_addr, pos);
        return pos;
    }
}