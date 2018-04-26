#pragma once

#include <functional>
#include <thread>
#include <future>

namespace skyfire
{
    template <typename _Ret>
    class sf_chain_call__
    {
    private:

        std::function<_Ret()> func__;

    public:

        sf_chain_call__(std::function<_Ret()> func) :
                func__(func)
        {
        };

        template <typename _R, typename ... _Params>
        sf_chain_call__<_R> then(std::function<_R(_Ret, _Params...)> func, _Params&& ... param) const
        {
            std::function<_R()> f = [=, &param ... ]() -> _R{
                func(func__(), std::forward<_Params>(param) ...);
            };
            return sf_chain_call__<_R> (f);
        }

        template <typename _R, typename ... _Params>
        sf_chain_call__<_R> then(_R(func)(_Ret, _Params...), _Params&& ... param) const
        {
            std::function<_R()> f = [=, &param ... ]() -> _R{
                func(func__(), std::forward<_Params>(param) ...);
            };
            return sf_chain_call__<_R> (f);
        }


        auto call() const
        {
            return func__();
        }

    };

    template <>
    class sf_chain_call__<void>
    {
    private:

        std::function<void()> func__;

    public:

        sf_chain_call__(std::function<void()> func) :
                func__(func)
        {
        };

        template <typename _R, typename ... _Params>
        sf_chain_call__<_R> then(std::function<_R(_Params...)> func, _Params&& ... param) const
        {
            std::function<_R()> f = [=, &param ... ]() -> _R{
                func__();
                func(std::forward<_Params>(param) ...);
            };
            return sf_chain_call__<_R> (f);
        }

        template <typename _R, typename ... _Params>
        sf_chain_call__<_R> then(_R(func)(_Params...), _Params&& ... param) const
        {
            std::function<_R()> f = [=, &param ... ]() -> _R{
                func__();
                func(std::forward<_Params>(param) ...);
            };
            return sf_chain_call__<_R> (f);
        }


        void call() const
        {
            func__();
        }
    };


    template <typename _Ret>
    class sf_chain_async_call__
    {
    private:
        std::future<_Ret> ret__;

    public:
        sf_chain_async_call__(std::shared_ptr<std::packaged_task<_Ret()>> task)
        {
            ret__ = task->get_future();
            std::thread(std::move(*task)).detach();
        }

        template <typename _R, typename ... _Params>
        sf_chain_async_call__<_R> then(std::function<_R(_Ret, _Params...)> func, _Params&& ... param)
        {
            auto task=std::make_shared<std::packaged_task<_R()>>(std::bind(func, ret__.get(), std::forward<_Params>(param) ...));
            return sf_chain_async_call__<_R>(task);
        }

        template <typename _R, typename ... _Params>
        sf_chain_async_call__<_R> then(_R(func)(_Ret, _Params...), _Params&& ... param)
        {
            auto task=std::make_shared<std::packaged_task<_R()>>(std::bind(func, ret__.get(), std::forward<_Params>(param) ...));
            return sf_chain_async_call__<_R>(task);
        }

        std::future<_Ret> get_future()
        {
            return ret__;
        }
    };


    template <>
    class sf_chain_async_call__<void>
    {
    private:

    public:
        sf_chain_async_call__(std::shared_ptr<std::packaged_task<void()>> task)
        {
            task->get_future();
            std::thread(std::move(*task)).detach();
        }

        template <typename _R, typename ... _Params>
        sf_chain_async_call__<_R> then(std::function<_R(_Params...)> func, _Params&& ... param)
        {
            auto task=std::make_shared<std::packaged_task<void()>>(std::bind(func, std::forward<_Params>(param) ...));
            return sf_chain_async_call__<_R>(task);
        }

        template <typename _R, typename ... _Params>
        sf_chain_async_call__<_R> then(_R(func)(_Params...), _Params&& ... param)
        {
            auto task=std::make_shared<std::packaged_task<void()>>(std::bind(func, std::forward<_Params>(param) ...));
            return sf_chain_async_call__<_R>(task);
        }

        std::future<void> get_future()
        {
            return std::future<void>();
        }
    };




    template <typename _Ret, typename ... _Args>
    sf_chain_call__<_Ret> make_chain_call(std::function<_Ret(_Args...)> func, _Args &&... args)
    {
        std::function<_Ret()> f = std::bind(func, std::forward<_Args>(args) ... );
        return sf_chain_call__(f);
    };

    template <typename _Ret, typename ... _Args>
    sf_chain_call__<_Ret> make_chain_call(_Ret(func)(_Args...), _Args &&... args)
    {
        std::function<_Ret()> f = std::bind(func, std::forward<_Args>(args) ... );
        return sf_chain_call__(f);
    };


    template <typename _Ret, typename ... _Args>
    sf_chain_async_call__<_Ret> make_chain_async_call(std::function<_Ret(_Args...)> func, _Args &&... args)
    {
        std::function<_Ret()> f = std::bind(func, std::forward<_Args>(args) ... );
        return sf_chain_async_call__(std::make_shared<std::packaged_task<_Ret()>>(f));
    };

    template <typename _Ret, typename ... _Args>
    sf_chain_async_call__<_Ret> make_chain_async_call(_Ret(func)(_Args...), _Args &&... args)
    {
        std::function<_Ret()> f = std::bind(func, std::forward<_Args>(args) ... );
        return sf_chain_async_call__(std::make_shared<std::packaged_task<_Ret()>>(f));
    };

}