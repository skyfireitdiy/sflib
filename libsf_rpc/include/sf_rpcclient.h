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

#ifdef SF_RPC_SERVER
#error You cannot include 'sf_rpcserver.h' and 'sf_rpcclient.h'
#endif

#define SF_RPC_CLIENT

#include "sf_rpcutils.h"

namespace skyfire
{
    template <typename _BaseClass = sf_empty_class>
    class sf_rpcclient : public sf_nocopy<_BaseClass>
    {
    protected:
        std::shared_ptr<sf_tcpclient> __tcp_client__ = sf_tcpclient::make_client();
        pkg_header_t __header__;
        byte_array  __data__;
        std::mutex __back_mu__;
        std::mutex __call_mu__;
        std::condition_variable __back_cond__;
        std::atomic<bool> __back_finished__ {false};
        std::mutex __read_mu__;
        std::condition_variable __read_cond__;

        void __back_callback(const pkg_header_t& header_t, const byte_array& data_t)
        {
            __header__ = header_t;
            __data__ = data_t;
            __back_finished__ = true;
            __back_cond__.notify_one();
            std::unique_lock<std::mutex> lck(__read_mu__);
            __read_cond__.wait(lck);
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
    };
}

#include "sf_rpcutils.h"