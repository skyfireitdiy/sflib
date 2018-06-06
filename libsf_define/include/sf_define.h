#pragma once

namespace skyfire{
    // TCP相关定义

    // 用户可用的起始位置
    constexpr int TCP_PKG_TYPE_USER = 0x70000000;

    // RPC掩码
    constexpr int TCP_RPC_TYPE_MASK = 0x0000ffff;


    // 以下为系统定义type类型
    constexpr int TYPE_NAT_TRAVERSAL_GET_LIST = 0x00010001;

    constexpr int TYPE_NAT_TRAVERSAL_LIST = 0x00010002;

    constexpr int TYPE_NAT_TRAVERSAL_REG = 0x00010003;




    constexpr size_t BUFFER_SIZE = 4096;
}