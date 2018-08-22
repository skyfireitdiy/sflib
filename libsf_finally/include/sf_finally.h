/*
 * sf_finally 清理过程
 */

#pragma once
#include <functional>

namespace skyfire
{
    class sf_finally{
    public:
        /**
         * @brief sf_finally 构造一个清理过程对象
         * @param func 作用域结束后会调用的函数
         */
        sf_finally(std::function<void()> func);
        ~sf_finally();
    private:
        std::function<void()> func__;
    };

}
