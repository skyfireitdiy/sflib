#pragma once

#include <functional>
#include <thread>

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
            std::function<_Ret()> f = [=, &param ... ]() -> _R{
                func(func__(), std::forward<_Params>(param) ...);
            };
            return sf_chain_call__<_R> (f);
        }

        template <typename _R, typename ... _Params>
        sf_chain_call__<_R> then(_R(func)(_Ret, _Params...), _Params&& ... param) const
        {
            std::function<_Ret()> f = [=, &param ... ]() -> _R{
                func(func__(), std::forward<_Params>(param) ...);
            };
            return sf_chain_call__<_R> (f);
        }


        auto call() const
        {
            return func__();
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

}