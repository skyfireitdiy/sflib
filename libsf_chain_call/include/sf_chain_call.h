#pragma once

#include <functional>

#include "sf_nocopy.h"

namespace skyfire
{
    template <typename _Ret, typename _Param ...>
    class sf_chain_call : public sf_nocopy<>
    {
    public:

        sf_chain_call(std::function<_Ret(_Param...)> func, _Param && ... param)
        {

        };


    private:
        std::function<_Ret(_Param ...)> func__;

    };
}