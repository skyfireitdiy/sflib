
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_finally.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
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
class sf_finally final {
   public:
    /**
     * @brief sf_finally 构造一个清理过程对象
     * @param func 作用域结束后会调用的函数
     */
    explicit sf_finally(std::function<void()> func);
    sf_finally(const sf_finally&) = delete;
    sf_finally(sf_finally&&) = delete;
    sf_finally& operator=(sf_finally&&) = delete;
    sf_finally& operator=(const sf_finally&) = delete;
    ~sf_finally();

   private:
    std::function<void()> func__;
};

}    // namespace skyfire
