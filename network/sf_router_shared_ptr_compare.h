
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_router_shared_ptr_compare.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

namespace skyfire {
/**
 * 路由智能指针比较
 * @tparam RouterType
 */
template <typename RouterType>
struct sf_router_shared_ptr_compare__ final {
    /**
     * 路由比较
     * @param router1 路由1智能指针
     * @param router2 路由2智能指针
     * @return
     */
    bool operator()(const std::shared_ptr<RouterType> &router1,
                    const std::shared_ptr<RouterType> &router2) const;
};
}    // namespace skyfire