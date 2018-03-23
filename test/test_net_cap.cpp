#include "sf_network_cap.h"

#include <iostream>
#include <map>

using namespace skyfire;
using namespace std;


#define ADD_PROTO(x) [x] = #x

map<int,string> proto_map;


void pkg_come(const byte_array& data)
{
    cout<<"#############BEGIN############"<<endl;
    cout<<"==============IP=============="<<endl;
    const ip_header* p_ip = reinterpret_cast<const ip_header*>(data.data());
    cout<<"Version:"<<static_cast<unsigned int>((p_ip->version_and_len >> 4) & 0x0f)<<endl;
    cout<<"Header Length:"<<static_cast<unsigned int>(p_ip->version_and_len & 0x0f)<<endl;
    cout<<"TOS:"<< static_cast<unsigned int>(p_ip->tos) <<endl;
    cout<<"Total length:"<< static_cast<unsigned int>(p_ip->length)<<endl;
    cout<<"ID:" << static_cast<unsigned int>(p_ip->id)<<endl;
    cout<<"Flag:" << static_cast<unsigned int>((p_ip->flag_and_offset >> 13)&0x07)<<endl;
    cout<<"Offset:" << static_cast<unsigned int>(p_ip->flag_and_offset&0x1fff)<<endl;
    cout<<"TTL:"<<static_cast<unsigned int>(p_ip->ttl)<<endl;
    cout<<"Protocol:"<<proto_map[p_ip->protocol]<<endl;
    cout<<"Checksum:"<<static_cast<unsigned int>(p_ip->checksum)<<endl;
    cout<<"Source IP:"<< inet_ntoa(*reinterpret_cast<const in_addr*>(&p_ip->src_ip))<<endl;
    cout<<"Destination IP:"<<inet_ntoa(*reinterpret_cast<const in_addr*>(&p_ip->dst_ip))<<endl;

    if(p_ip->protocol == IPPROTO_TCP)
    {
        cout<<"==============TCP============="<<endl;
        const tcp_header *p_tcp = reinterpret_cast<const tcp_header *>(data.data() +
                                                                       (p_ip->version_and_len & 0x0f) * 4);
        cout<<"Source port:"<< ntohs(p_tcp->src_port)<<endl;
        cout<<"Destination port:"<< ntohs(p_tcp->dst_port)<<endl;
        cout<<"Sequ number:"<< static_cast<unsigned int>(p_tcp->seq_num)<<endl;
        cout<<"ACK sequ number:"<< static_cast<unsigned int>(p_tcp->ack_seq_num)<<endl;
        cout<<"Header length:"<< static_cast<unsigned int>((p_tcp->header_length_and_flags>>12) & 0x0f)<<endl;
        cout<<"URG:"<<static_cast<unsigned int>(p_tcp->header_length_and_flags & 0b00100000)<<endl;
        cout<<"ACK:"<<static_cast<unsigned int>(p_tcp->header_length_and_flags & 0b00010000)<<endl;
        cout<<"PSH:"<<static_cast<unsigned int>(p_tcp->header_length_and_flags & 0b00001000)<<endl;
        cout<<"PST:"<<static_cast<unsigned int>(p_tcp->header_length_and_flags & 0b00000100)<<endl;
        cout<<"SYN:"<<static_cast<unsigned int>(p_tcp->header_length_and_flags & 0b00000010)<<endl;
        cout<<"FIN:"<<static_cast<unsigned int>(p_tcp->header_length_and_flags & 0b00000001)<<endl;
        cout<<"Window size:"<< static_cast<unsigned int>(p_tcp->window_size)<<endl;
        cout<<"Checksum:"<< static_cast<unsigned int>(p_tcp->checksum)<<endl;
        cout<<"Urgent pointer:"<< static_cast<unsigned int>(p_tcp->urgent_pointer)<<endl;
    }
    else if (p_ip->protocol == IPPROTO_UDP)
    {
        cout<<"==============UDP============="<<endl;
        const udp_header *p_udp = reinterpret_cast<const udp_header *>(data.data() +
                                                                       (p_ip->version_and_len & 0x0f) * 4);
        cout<<"Source port:"<< ntohs(p_udp->src_port)<<endl;
        cout<<"Destination port:"<< ntohs(p_udp->dst_port)<<endl;
        cout<<"Package length:"<< static_cast<unsigned int>(p_udp->length)<<endl;
        cout<<"Checksum:"<< static_cast<unsigned int>(p_udp->checksum)<<endl;
    }
    cout<<"##############END#############"<<endl;
}

int main()
{

    proto_map ADD_PROTO(IPPROTO_IP);
    proto_map ADD_PROTO(IPPROTO_HOPOPTS);
    proto_map ADD_PROTO(IPPROTO_ICMP);
    proto_map ADD_PROTO(IPPROTO_IGMP);
    proto_map ADD_PROTO(IPPROTO_GGP);
    proto_map ADD_PROTO(IPPROTO_IPV4);
    proto_map ADD_PROTO(IPPROTO_TCP);
    proto_map ADD_PROTO(IPPROTO_PUP);
    proto_map ADD_PROTO(IPPROTO_UDP);
    proto_map ADD_PROTO(IPPROTO_IDP);
    proto_map ADD_PROTO(IPPROTO_IPV6);
    proto_map ADD_PROTO(IPPROTO_ROUTING);
    proto_map ADD_PROTO(IPPROTO_FRAGMENT);
    proto_map ADD_PROTO(IPPROTO_ESP);
    proto_map ADD_PROTO(IPPROTO_AH);
    proto_map ADD_PROTO(IPPROTO_ICMPV6);
    proto_map ADD_PROTO(IPPROTO_NONE);
    proto_map ADD_PROTO(IPPROTO_DSTOPTS);
    proto_map ADD_PROTO(IPPROTO_ND);
    proto_map ADD_PROTO(IPPROTO_ICLFXBM);
    proto_map ADD_PROTO(IPPROTO_RAW);
    proto_map ADD_PROTO(IPPROTO_MAX);



    shared_ptr<sf_network_cap> p_cap = make_shared<sf_network_cap>();
    sf_bind_signal(p_cap,data_caped,pkg_come,true);
    p_cap->start("192.168.7.125");
    sf_eventloop loop;
    loop.exec();
}