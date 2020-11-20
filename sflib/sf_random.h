
/**
* @version 1.0.0
* @author skyfire
* @file sf_random.h
*/

#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedGlobalDeclarationInspection"

#include "sf_nocopy.h"
#include "sf_single_instance.h"
#include "sf_stdc++.h"

namespace skyfire
{
/**
 *  @brief 随机数类
 */
class random final : public nocopy<>
{
public:
    SF_SINGLE_TON(random)

private:
    std::random_device                          rd__;
    std::shared_ptr<std::default_random_engine> e__;

    random();

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

} // namespace skyfire
#pragma clang diagnostic pop