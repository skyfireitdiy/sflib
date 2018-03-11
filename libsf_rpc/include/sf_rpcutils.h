#pragma once

#ifndef SF_RPC_SERVER
#ifndef SF_RPC_CLIENT
#error You cannot include 'sf_rpcutils.h' before include 'sf_rpcclient.h' or 'sf_rpcserver.h'
#endif
#endif


#define SF_BEGIN_REG_RPC_SERVER_FUNC void __rpc_callback__(SOCKET s, int id_code, const std::string &id_str,const byte_array& data) {
#define SF_END_REG_RPC_SERVER_FUNC }


#define SF_REG_RPC_SERVER_FUNC(id,disname,func,ret_type,...)                          \
    if(id_str == id)                                                                  \
    {                                                                                 \
        std::tuple<__VA_ARGS__> param;                                                \
        sf_deserialize(data, param, 0);                                               \
        ret_type ret = __invoke(func, param);                                         \
        __send_back(s, id_code, id_str, ret);                                         \
    }                                                                                 \



#define SF_BEGIN_REG_RPC_CLIENT_FUNC
#define SF_END_REG_RPC_CLIENT_FUNC

#include "sf_finally.h"


#define SF_REG_RPC_CLIENT_FUNC(id,disname,func,ret_type,...)                                                                    \
template <typename ... __SF_RPC_ARGS__>                                                                                         \
ret_type disname(__SF_RPC_ARGS__ ... args)                                                                                      \
{                                                                                                                               \
    std::lock_guard<std::mutex> lck(__call_mu__);                                                                               \
    static_assert(std::is_same<std::tuple<__VA_ARGS__>,std::tuple<__SF_RPC_ARGS__...>>::value);                                 \
    std::tuple<__SF_RPC_ARGS__...> param{args...};                                                                              \
    int id_code = rand();                                                                                                       \
    pkg_header_t header;                                                                                                        \
    byte_array data;                                                                                                            \
    std::condition_variable cond_coming;                                                                                        \
    std::mutex mu_coming;                                                                                                       \
    std::atomic<bool> finished{false};                                                                                          \
    std::mutex mu_finish;                                                                                                       \
    __tcp_client__->send(id_code, sf_serialize(std::string(id))+sf_serialize(param));                                           \
    if(!__back_finished__)                                                                                                      \
    {                                                                                                                           \
        std::unique_lock<std::mutex> lck(__back_mu__);                                                                          \
        __back_cond__.wait(lck);                                                                                                \
    }                                                                                                                           \
    ret_type ret;                                                                                                               \
    std::string id_str;                                                                                                         \
    size_t pos = sf_deserialize(__data__, id_str, 0);                                                                           \
    sf_deserialize(__data__, ret, pos);                                                                                         \
    __read_cond__.notify_one();                                                                                                 \
    return ret;                                                                                                                 \
}


#ifdef SF_RPC_SERVER
#define SF_BEGIN_REG_RPC_FUNC SF_BEGIN_REG_RPC_SERVER_FUNC
#define SF_END_REG_RPC_FUNC SF_END_REG_RPC_SERVER_FUNC
#define SF_REG_RPC_FUNC SF_REG_RPC_SERVER_FUNC
#endif

#ifdef SF_RPC_CLIENT
#define SF_BEGIN_REG_RPC_FUNC SF_BEGIN_REG_RPC_CLIENT_FUNC
#define SF_END_REG_RPC_FUNC SF_END_REG_RPC_CLIENT_FUNC
#define SF_REG_RPC_FUNC SF_REG_RPC_CLIENT_FUNC
#endif