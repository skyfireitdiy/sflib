#pragma once

#include "sf_tcpclient.h"
#include "sf_nocopy.h"
#include "sf_serialize.h"
#include "sf_event_waiter.h"
#include <string>
#include <functional>
#include <tuple>
#include <memory>
#include <mutex>
#include <condition_variable>


namespace skyfire
{

    struct rpc_struct
    {
        pkg_header_t header;
        byte_array data;
        std::mutex back_mu;
        std::condition_variable back_cond;
        std::atomic<bool> back_finished;
        bool is_async;
        std::function<void(const byte_array&)> async_callback;
    };

    template <typename _BaseClass = sf_empty_class>
    class sf_rpcclient : public sf_nocopy<_BaseClass>
    {
    protected:
        std::shared_ptr<sf_tcpclient> __tcp_client__ = sf_tcpclient::make_client();
        std::map<int, std::shared_ptr<rpc_struct>> __rpc_data__;

        void __back_callback(const pkg_header_t& header_t, const byte_array& data_t)
        {
            int call_id = header_t.type;
            if(__rpc_data__[call_id]->is_async)
            {
                __rpc_data__[call_id]->async_callback(data_t);
                __rpc_data__.erase(call_id);
            }
            else
            {
                __rpc_data__[call_id]->header = header_t;
                __rpc_data__[call_id]->data = data_t;
                __rpc_data__[call_id]->back_finished = true;
                __rpc_data__[call_id]->back_cond.notify_one();
            }
        }
    public:
        sf_rpcclient()
        {
            sf_bind_signal(__tcp_client__,
                           data_coming,
                           std::bind(__back_callback,
                                                                  this,
                                                                  std::placeholders::_1,
                                                                  std::placeholders::_2),
                           true);
        }

        bool connect(const std::string ip, unsigned short port)
        {
            return __tcp_client__->connect(ip,port);
        }

        void close()
        {
            __tcp_client__->close();
        }

        template<typename _Ret, typename ... __SF_RPC_ARGS__>
        _Ret call(const std::string& func_id, const __SF_RPC_ARGS__ &... args)
        {
            std::tuple<__SF_RPC_ARGS__...> param{args...};
            int call_id = rand();
            pkg_header_t header;
            byte_array data;

            __rpc_data__[call_id] = std::make_shared<rpc_struct>();
            __rpc_data__[call_id]->is_async = false;
            __tcp_client__->send(call_id, sf_serialize(std::string(func_id)) + sf_serialize(param));
            if (!__rpc_data__[call_id]->back_finished)
            {
                std::unique_lock<std::mutex> lck(__rpc_data__[call_id]->back_mu);
                __rpc_data__[call_id]->back_cond.wait(lck);
            }

            std::string id_str;
            if constexpr (std::is_same<_Ret,void>::value)
            {
                return ;
            }
            else
            {
                _Ret ret;
                size_t pos = sf_deserialize(__rpc_data__[call_id]->data, id_str, 0);
                sf_deserialize(__rpc_data__[call_id]->data, ret, pos);
                __rpc_data__.erase(call_id);
                return ret;
            }
        }

        template<typename T>
        void async_call(const std::string& func_id,
                        std::function<void()> rpc_callback
        )
        {
            int call_id = rand();
            pkg_header_t header;
            byte_array data;
            __rpc_data__[call_id] = std::make_shared<rpc_struct>();
            __rpc_data__[call_id]->is_async = true;
            __rpc_data__[call_id]->async_callback = [=](const byte_array& data){
                rpc_callback();
            };
            __tcp_client__->send(call_id, sf_serialize(std::string(func_id)));
        }


        template<typename ... __SF_RPC_ARGS__>
        void async_call(const std::string& func_id,
                        std::function<void()> rpc_callback,
                        __SF_RPC_ARGS__ ...args
        )
        {
            std::tuple<__SF_RPC_ARGS__...> param{args...};
            int call_id = rand();
            pkg_header_t header;
            byte_array data;
            __rpc_data__[call_id] = std::make_shared<rpc_struct>();
            __rpc_data__[call_id]->is_async = true;
            __rpc_data__[call_id]->async_callback = [=](const byte_array& data){
                    rpc_callback();
            };
            __tcp_client__->send(call_id, sf_serialize(std::string(func_id)) + sf_serialize(param));
        }

        template<typename _Ret, typename ... __SF_RPC_ARGS__>
        void async_call(const std::string& func_id,
                        std::function<void(_Ret)> rpc_callback,
                        __SF_RPC_ARGS__ ...args
        )
        {
            std::tuple<__SF_RPC_ARGS__...> param{args...};
            int call_id = rand();
            pkg_header_t header;
            byte_array data;
            __rpc_data__[call_id] = std::make_shared<rpc_struct>();
            __rpc_data__[call_id]->is_async = true;
            __rpc_data__[call_id]->async_callback = [=](const byte_array& data)
            {
                _Ret ret;
                std::string id_str;
                size_t pos = sf_deserialize(data, id_str, 0);
                sf_deserialize(data, ret, pos);
                rpc_callback(ret);
            };
            __tcp_client__->send(call_id, sf_serialize(std::string(func_id)) + sf_serialize(param));
        }
    };
}
