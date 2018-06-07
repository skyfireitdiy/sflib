#pragma once

namespace skyfire{
    // TCP相关定义

    // 用户可用的起始位置
    constexpr int TCP_PKG_TYPE_USER = 0x70000000;

    // RPC掩码
    constexpr int TCP_RPC_TYPE_MASK = 0x0000ffff;


    // 以下为系统定义type类型
    // NAT穿透获取客户端列表
    constexpr int TYPE_NAT_TRAVERSAL_GET_LIST = 0x00010001;

    // NAT穿透客户端列表
    constexpr int TYPE_NAT_TRAVERSAL_LIST = 0x00010002;

    // NAT穿透客户端注册
    constexpr int TYPE_NAT_TRAVERSAL_REG = 0x00010003;

    // NAT穿透请求连接远端peer
    constexpr int TYPE_NAT_TRAVERSAL_REQUIRE_CONNECT_PEER = 0x00010004;

    // 网络缓冲区大小
    constexpr size_t SF_NET_BUFFER_SIZE = 4096;
}