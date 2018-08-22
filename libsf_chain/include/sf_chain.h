/*
 * 提供链式调用接口
 */

#pragma once

#include <functional>
#include <thread>
#include <future>

namespace skyfire {
    template<typename _Ret>
    class sf_chain_call__ {
    private:

        std::function<_Ret()> func__;

    public:

        sf_chain_call__(std::function<_Ret()> func);

        template<typename _R, typename ... _Params>
        sf_chain_call__<_R> then(std::function<_R(_Ret, _Params...)> func, _Params &&... param) const;

        template<typename _R, typename ... _Params>
        sf_chain_call__<_R> then(_R(func)(_Ret, _Params...), _Params &&... param) const;

        auto call() const ;

    };

    template<>
    class sf_chain_call__<void> {
    private:

        std::function<void()> func__;

    public:

        sf_chain_call__(std::function<void()> func);;

        template<typename _R, typename ... _Params>
        sf_chain_call__<_R> then(std::function<_R(_Params...)> func, _Params &&... param) const;

        template<typename _R, typename ... _Params>
        sf_chain_call__<_R> then(_R(func)(_Params...), _Params &&... param) const;

        void call() const;
    };


    template<typename _Ret>
    class sf_chain_async_call__ {
    private:
        std::future<_Ret> ret__;

    public:
        sf_chain_async_call__(std::shared_ptr<std::packaged_task<_Ret()>> task);

        template<typename _R, typename ... _Params>
        sf_chain_async_call__<_R> then(std::function<_R(_Ret, _Params...)> func, _Params &&... param);

        template<typename _R, typename ... _Params>
        sf_chain_async_call__<_R> then(_R(func)(_Ret, _Params...), _Params &&... param) {
            auto task = std::make_shared<std::packaged_task<_R()>>(
                    std::bind(func, ret__.get(), std::forward<_Params>(param) ...));
            return sf_chain_async_call__<_R>(task);
        }

        std::future<_Ret> get_future() {
            return ret__;
        }
    };




    template<>
    class sf_chain_async_call__<void> {
    private:

    public:
        sf_chain_async_call__(std::shared_ptr<std::packaged_task<void()>> task);

        template<typename _R, typename ... _Params>
        sf_chain_async_call__<_R> then(std::function<_R(_Params...)> func, _Params &&... param);

        template<typename _R, typename ... _Params>
        sf_chain_async_call__<_R> then(_R(func)(_Params...), _Params &&... param);

        std::future<void> get_future();
    };




    /**
     * @brief make_chain_call 初始化一个链式调用
     * @param func function对象
     * @param args 参数
     * @return 链式调用对象
     */
    template<typename _Ret, typename ... _Args>
    sf_chain_call__<_Ret> make_chain_call(std::function<_Ret(_Args...)> func, _Args &&... args);



    /**
     * @brief make_chain_call 初始化一个链式调用
     * @param func 函数
     * @param args 参数
     * @return 链式调用对象
     */
    template<typename _Ret, typename ... _Args>
    sf_chain_call__<_Ret> make_chain_call(_Ret(func)(_Args...), _Args &&... args);


    /**
     * @brief make_chain_async_call 创建一个异步链式调用
     * @param func function 对象
     * @param args 参数
     * @return 链式调用对象
     */
    template<typename _Ret, typename ... _Args>
    sf_chain_async_call__<_Ret> make_chain_async_call(std::function<_Ret(_Args...)> func, _Args &&... args);



    /**
     * @brief make_chain_async_call 创建一个异步链式调用
     * @param func function 对象
     * @param args 参数
     * @return 链式调用对象
     */
    template<typename _Ret, typename ... _Args>
    sf_chain_async_call__<_Ret> make_chain_async_call(_Ret(func)(_Args...), _Args &&... args);


}
