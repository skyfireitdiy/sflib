#pragma once

namespace skyfire
{
    constexpr int NETWORK_RAW_BUFFER_SIZE = 65536;

    struct alignas(1) ip_header
    {
        unsigned char version_and_len;
        unsigned char tos;
        unsigned short length;
        unsigned short id;
        unsigned short flag_and_offset;
        unsigned char ttl;
        unsigned char protocol;
        unsigned short checksum;
        unsigned int src_ip;
        unsigned int dst_ip;
    };

    struct alignas(1) tcp_header
    {
        unsigned short src_port;
        unsigned short dst_port;
        unsigned int seq_num;
        unsigned int ack_seq_num;
        unsigned short header_length_and_flags;
        unsigned short window_size;
        unsigned short checksum;
        unsigned short urgent_pointer;
    };

    struct udp_header
    {
        unsigned short src_port;
        unsigned short dst_port;
        unsigned short length;
        unsigned short checksum;
    };


}