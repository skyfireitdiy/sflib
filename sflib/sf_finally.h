
/**
* @version 1.0.0
* @author skyfire
* @file sf_finally.h
*/

/*
 * sf_finally 清理过程
 */

#pragma once
#include <functional>

namespace skyfire {
/**
 *  @brief 清理过程对象
 */
class __sf_finally__ final {
   public:
    /**
     * @brief sf_finally 构造一个清理过程对象
     * @param func 作用域结束后会调用的函数
     */
    explicit __sf_finally__(std::function<void()> func);
    __sf_finally__(const __sf_finally__&) = delete;
    __sf_finally__(__sf_finally__&&) = delete;
    __sf_finally__& operator=(__sf_finally__&&) = delete;
    __sf_finally__& operator=(const __sf_finally__&) = delete;
    ~__sf_finally__();

private:
    std::function<void()> func__;
};

}    // namespace skyfire

#define __SF_VAR_NAME_WP__(prefix, line) prefix##line
#define __SF_VAR_NAME__(prefix, line) __SF_VAR_NAME_WP__(prefix, line)
#define sf_finally(x) skyfire::__sf_finally__ __SF_VAR_NAME__(__SF_FINALLY_N_, __LINE__)(x)