
#pragma once
#include "nocopy.h"
#include "single_instance.h"
#include "stdc++.h"
namespace skyfire
{
class random final : public nocopy<>
{
public:
    SF_SINGLE_TON(random)
private:
    std::random_device                          rd__;
    std::shared_ptr<std::default_random_engine> e__;
    random();

public:
    int         rand_int(int min, int max) const;
    double      rand_double(double min, double max) const;
    std::string uuid_str() const;
};
} // namespace skyfire
