
/**
* @version 1.0.0
* @author skyfire
* @file sf_random.hpp
*/
#pragma once

#include "sf_random.h"

namespace skyfire
{
inline random::random()
{
    e__ = std::make_shared<std::default_random_engine>(rd__());
}

inline int random::rand_int(const int min, const int max) const
{
    std::uniform_int_distribution<int> ed(min, max);
    return ed(*e__.get());
}

inline double random::rand_double(const double min, const double max) const
{
    std::uniform_real_distribution<double> ed(min, max);
    return ed(*e__.get());
}

inline std::string random::uuid_str() const
{
    std::uniform_int_distribution<int> ed(INT_MIN, INT_MAX);
    int                                data[4] { ed(*e__.get()), ed(*e__.get()), ed(*e__.get()), ed(*e__.get()) };
    char                               c_str[40];
    // FIXME 溢出
    sprintf(c_str, "%0X%0X%0X%0X", data[0], data[1], data[2],
                 data[3]);
    std::string ret = c_str;
    ret.insert(ret.begin() + 20, '-');
    ret.insert(ret.begin() + 16, '-');
    ret.insert(ret.begin() + 12, '-');
    ret.insert(ret.begin() + 8, '-');
    return ret;
}
} // namespace skyfire