#pragma once

#ifdef __GNUC__
#if __GNUC__ < 7
#error Please update your complier!
#endif
#endif

#include <cctype>

#define SF_PRIVATE_ARGS_GLUE(x, y) x y
#define SF_PRIVATE_MACRO_VAR_ARGS_IMPL_COUNT(_1,_2,_3,_4,_5,_6,_7,_8,_9, _10, N, ...) N
#define SF_PRIVATE_MACRO_VAR_ARGS_IMPL(args) SF_PRIVATE_MACRO_VAR_ARGS_IMPL_COUNT args
#define SF_COUNT_MACRO_VAR_ARGS(...) SF_PRIVATE_MACRO_VAR_ARGS_IMPL((__VA_ARGS__,10, 9,8,7,6,5,4,3,2,1,0))

#define SF_PRIVATE_MACRO_CHOOSE_HELPER2(M,count)  M##count
#define SF_PRIVATE_MACRO_CHOOSE_HELPER1(M,count) SF_PRIVATE_MACRO_CHOOSE_HELPER2(M,count)
#define SF_PRIVATE_MACRO_CHOOSE_HELPER(M,count)  SF_PRIVATE_MACRO_CHOOSE_HELPER1(M,count)

#define SF_EXPAND_ARGS_2(prefix, _1) prefix._1,
#define SF_EXPAND_ARGS_3(prefix, _1, _2) prefix._1,prefix._2
#define SF_EXPAND_ARGS_4(prefix, _1, _2, _3) prefix._1,prefix._2, prefix._3
#define SF_EXPAND_ARGS_5(prefix, _1, _2, _3, _4) prefix._1,prefix._2, prefix._3, prefix._4
#define SF_EXPAND_ARGS_6(prefix, _1, _2, _3, _4, _5) prefix._1,prefix._2, prefix._3, prefix._4, prefix._5
#define SF_EXPAND_ARGS_7(prefix, _1, _2, _3, _4, _5, _6) prefix._1,prefix._2, prefix._3, prefix._4, prefix._5, prefix._6
#define SF_EXPAND_ARGS_8(prefix, _1, _2, _3, _4, _5, _6, _7) prefix._1,prefix._2, prefix._3, prefix._4, prefix._5, prefix._6, prefix._7
#define SF_EXPAND_ARGS_9(prefix, _1, _2, _3, _4, _5, _6, _7, _8) prefix._1,prefix._2, prefix._3, prefix._4, prefix._5, prefix._6, prefix._7, prefix._8
#define SF_EXPAND_ARGS_10(prefix, _1, _2, _3, _4, _5, _6, _7, _8, _9) prefix._1,prefix._2, prefix._3, prefix._4, prefix._5, prefix._6, prefix._7, prefix._8, prefix._9
#define SF_EXPAND_ARGS_11(prefix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) prefix._1,prefix._2, prefix._3, prefix._4, prefix._5, prefix._6, prefix._7, prefix._8, prefix._9,prefix._10
#define SF_EXPAND_ARGS_12(prefix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11) prefix._1,prefix._2, prefix._3, prefix._4, prefix._5, prefix._6, prefix._7, prefix._8, prefix._9,prefix._10, prefix._11
#define SF_EXPAND_ARGS_13(prefix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12) prefix._1,prefix._2, prefix._3, prefix._4, prefix._5, prefix._6, prefix._7, prefix._8, prefix._9,prefix._10, prefix._11, prefix._12
#define SF_EXPAND_ARGS_14(prefix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13) prefix._1,prefix._2, prefix._3, prefix._4, prefix._5, prefix._6, prefix._7, prefix._8, prefix._9,prefix._10, prefix._11, prefix._12, prefix._13
#define SF_EXPAND_ARGS_15(prefix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14) prefix._1,prefix._2, prefix._3, prefix._4, prefix._5, prefix._6, prefix._7, prefix._8, prefix._9,prefix._10, prefix._11, prefix._12, prefix._13, prefix._14
#define SF_EXPAND_ARGS_16(prefix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15) prefix._1,prefix._2, prefix._3, prefix._4, prefix._5, prefix._6, prefix._7, prefix._8, prefix._9,prefix._10, prefix._11, prefix._12, prefix._13, prefix._14, prefix._15
#define SF_EXPAND_ARGS_17(prefix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16) prefix._1,prefix._2, prefix._3, prefix._4, prefix._5, prefix._6, prefix._7, prefix._8, prefix._9,prefix._10, prefix._11, prefix._12, prefix._13, prefix._14, prefix._15, prefix._16
#define SF_EXPAND_ARGS_18(prefix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17) prefix._1,prefix._2, prefix._3, prefix._4, prefix._5, prefix._6, prefix._7, prefix._8, prefix._9,prefix._10, prefix._11, prefix._12, prefix._13, prefix._14, prefix._15, prefix._16, prefix._17
#define SF_EXPAND_ARGS_19(prefix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18) prefix._1,prefix._2, prefix._3, prefix._4, prefix._5, prefix._6, prefix._7, prefix._8, prefix._9,prefix._10, prefix._11, prefix._12, prefix._13, prefix._14, prefix._15, prefix._16, prefix._17,prefix._18
#define SF_EXPAND_ARGS_20(prefix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19) prefix._1,prefix._2, prefix._3, prefix._4, prefix._5, prefix._6, prefix._7, prefix._8, prefix._9,prefix._10, prefix._11, prefix._12, prefix._13, prefix._14, prefix._15, prefix._16, prefix._17,prefix._18, prefix._19
#define SF_EXPAND_ARGS_21(prefix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20) prefix._1,prefix._2, prefix._3, prefix._4, prefix._5, prefix._6, prefix._7, prefix._8, prefix._9,prefix._10, prefix._11, prefix._12, prefix._13, prefix._14, prefix._15, prefix._16, prefix._17,prefix._18, prefix._19, prefix._20

#define SF_INVOKE_VAR_MACRO(M,...) SF_PRIVATE_ARGS_GLUE(SF_PRIVATE_MACRO_CHOOSE_HELPER(M,SF_COUNT_MACRO_VAR_ARGS(__VA_ARGS__)), (__VA_ARGS__))

#define SF_EXPAND_OBJ_MEM(...) SF_INVOKE_VAR_MACRO(SF_EXPAND_ARGS_, __VA_ARGS__)

namespace skyfire {


    // 网络缓冲区大小
    constexpr size_t SF_NET_BUFFER_SIZE = 4096;


    ///////////////////////////////////////////////////////////////////////////////////////////////////

    // TCP相关定义

    // 用户可用的起始位置
    constexpr int TCP_PKG_TYPE_USER = 0x70000000;


    constexpr int RPC_REQ_TYPE = 0x0000ffff;


    constexpr int RPC_RES_TYPE = 0x0000fffe;




    // 以下为系统定义type类型
    // NAT穿透获取客户端列表
    enum sf_nat_pkg_type
    {
        TYPE_NAT_TRAVERSAL_GET_LIST = 0x00010001,

        // NAT穿透客户端列表
        TYPE_NAT_TRAVERSAL_LIST = 0x00010002,

        // NAT穿透客户端注册
        TYPE_NAT_TRAVERSAL_REG = 0x00010003,

        // NAT穿透请求连接远端peer
        TYPE_NAT_TRAVERSAL_REQUIRE_CONNECT_PEER = 0x00010004,

        // NAT设置client id
        TYPE_NAT_TRAVERSAL_SET_ID = 0x00010005,

        // NAT穿透错误
        TYPE_NAT_TRAVERSAL_ERROR = 0x00010005,

        // NAT穿透新连接请求
        TYPE_NAT_TRAVERSAL_NEW_CONNECT_REQUIRED = 0x00010006,

        // NAT穿透服务器回复B的地址
        TYPE_NAT_TRAVERSAL_SERVER_REPLY_B_ADDR = 0x00010007,

        // NAT穿透B端回复地址
        TYPE_NAT_TRAVERSAL_B_REPLY_ADDR = 0x00010008,

        // NAT穿透连接成功
        TYPE_NAT_TRAVERSAL_CONNECTED = 0x00010009
    };


    // http
    //////////////////////////////////////////////////////////////////////////////////////////////////
    // http静态目录路由优先级
    constexpr int DEFAULT_HTTP_STATIC_PRIORITY  = 100;
    // 默认http文件最大大小1M
    constexpr unsigned long long DEFAULT_HTTP_MAX_FILE_SIZE = 1024*1024;

///////////////////////////////////////////////////////////////////////////////////////////////////

    // error_code

    enum sf_error_code
    {
        // 无错误
        SF_ERR_OK = 0,

        // 资源不存在
        SF_ERR_NOT_EXIST = 0x00000001,

        // 连接断开
        SF_ERR_DISCONNECT = 0x00010002,

        // 远程错误
        SF_ERR_REMOTE_ERR = 0x00010003,

        // 监听错误
        SF_ERR_LISTEN_ERR = 0x00010004,

        // 绑定错误
        SF_ERR_BIND_ERR = 0x00010005,
    };
}








