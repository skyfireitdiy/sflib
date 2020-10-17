
/**
* @version 1.0.0
* @author skyfire
* @file sf_finally.h
*/

/*
 * finally 清理过程
 */

#pragma once
#include <functional>

namespace skyfire {
/**
 *  @brief 清理过程对象
 */
class __finally__ final {
   public:
    /**
     * @brief finally 构造一个清理过程对象
     * @param func 作用域结束后会调用的函数
     */
    explicit __finally__(std::function<void()> func);
    __finally__(const __finally__&) = delete;
    __finally__(__finally__&&) = delete;
    __finally__& operator=(__finally__&&) = delete;
    __finally__& operator=(const __finally__&) = delete;
    ~__finally__();

private:
    std::function<void()> func__;
};

}    // namespace skyfire

#define __SF_VAR_NAME_WP__(prefix, line) prefix##line
#define __SF_VAR_NAME__(prefix, line) __SF_VAR_NAME_WP__(prefix, line)
#define finally(...) skyfire::__finally__ __SF_VAR_NAME__(__SF_FINALLY_N_, __LINE__)(std::function<void()>(__VA_ARGS__))