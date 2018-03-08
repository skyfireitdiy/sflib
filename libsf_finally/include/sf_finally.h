#pragma once
#include <functional>

namespace skyfire
{
    class sf_finally{
    public:
        sf_finally(std::function<void()> func):func__(func)
        {
        }
        ~sf_finally(){
            func__();
        }
    private:
        std::function<void()> func__;
    };
}