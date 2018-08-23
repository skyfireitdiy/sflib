/*
 * sf_rpcclient rpc客户端
 */

#pragma once

#include "sf_tcpclient.hpp"
#include "sf_nocopy.hpp"
#include "sf_serialize_binary.hpp"
#include "sf_timer.hpp"
#include "sf_tri_type.h"
#include "sf_define.hpp"
#include <string>
#include <functional>
#include <tuple>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <chrono>


namespace skyfire {

    struct rpc_struct {
        pkg_header_t header;
        byte_array data;
        std::mutex back_mu;
        std::condition_variable back_cond;
        std::atomic<bool> back_finished;
        bool is_async;
        std::function<void(const byte_array &)> async_callback;
    };

    template<typename _BaseClass = sf_empty_class>
    class sf_rpcclient : public sf_nocopy<_BaseClass> {
    private:
        std::shared_ptr<sf_tcpclient> __tcp_client__ = sf_tcpclient::make_client();
        std::map<int, std::shared_ptr<rpc_struct>> __rpc_data__;

        int current_call_id__ = 0;
        unsigned int rpc_timeout__ = 30000;

        int __make_call_id();

        void __back_callback(const pkg_header_t &header_t, const byte_array &data_t);

        void __on_closed();

    public:
        /**
         * @brief make_client 创建RPC客户端
         * @return 客户端对象
         */
        static std::shared_ptr<sf_rpcclient> make_client();

        /**
         * @brief sf_rpcclient 构造RPC客户端
         */
        sf_rpcclient();

        /**
         * @brief set_rpc_timeout 设置RPC超时
         * @param ms 超时毫秒数
         */
        void set_rpc_timeout(unsigned int ms);

        /**
         * @brief connect 连接RPC服务端
         * @param ip ip
         * @param port 端口
         * @return 是否连接成功
         */
        bool connect(const std::string ip, unsigned short port);

        /**
         * @brief close 关闭RPC客户端
         */
        void close();

        /**
          * @brief call 调用
          * @param func_id 函数id
          * @param args 参数列表
          * @return 返回值
          */
        template<typename _Ret=void, typename ... __SF_RPC_ARGS__>
        sf_tri_type<_Ret> call(const std::string &func_id, __SF_RPC_ARGS__ ... args);

        /**
          * @brief async_call 异步调用
          * @param func_id 函数id
          * @param rpc_callback 返回后的回调函数
          */
        template<typename T>
        void async_call(const std::string &func_id,
                        std::function<void()> rpc_callback
        );

        /**
          * @brief async_call 异步调用（无返回）
          * @param func_id 函数id
          * @param rpc_callback 返回后的回调函数
          * @param args 参数
          */
        template<typename ... __SF_RPC_ARGS__>
        void async_call(const std::string &func_id,
                        std::function<void()> rpc_callback,
                        __SF_RPC_ARGS__ ...args
        );

        /**
          * @brief async_call 异步调用（有返回）
          * @param func_id 函数id
          * @param rpc_callback 返回后的回调函数
          * @param args 参数
          */
        template<typename _Ret, typename ... __SF_RPC_ARGS__>
        void async_call(const std::string &func_id,
                        std::function<void(_Ret)> rpc_callback,
                        __SF_RPC_ARGS__ ...args
        );
    };
}
