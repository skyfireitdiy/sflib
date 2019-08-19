
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_random.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include "core/sf_stdc++.h"
#include "sf_single_instance.h"
#include "tools/sf_nocopy.h"

namespace skyfire {
/**
 *  @brief 随机数类
 */
class sf_random final : public sf_nocopy<> {
   public:
    SF_SINGLE_TON(sf_random)

   private:
    std::random_device rd__;
    std::shared_ptr<std::default_random_engine> e__;

    sf_random();

   public:
    /**
     * 获取随机整数
     * @param min 最小值
     * @param max 最大值
     * @return 随机数
     */
    int rand_int(int min, int max) const;
    /**
     * 获取随机浮点数
     * @param min 最小值
     * @param max 最大值
     * @return 随机数
     */
    double rand_double(double min, double max) const;
    /**
     * 获取uuid字符串
     * @return uuid字符串
     */
    std::string uuid_str() const;
};

}    // namespace skyfire
#pragma clang diagnostic pop