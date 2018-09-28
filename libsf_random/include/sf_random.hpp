#pragma once

#include "sf_random.h"

namespace skyfire {
    inline sf_random::sf_random() {
        e__ = std::make_shared<std::default_random_engine>(rd__());
    }

    inline int sf_random::get_int(int min, int max) {
        std::uniform_int_distribution<int> ed(min, max);
        return ed(*e__.get());
    }

    inline double sf_random::get_double(double min, double max) {
        std::uniform_real_distribution<double > ed(min, max);
        return ed(*e__.get());
    }

    inline std::string sf_random::get_uuid_str()
    {
        std::uniform_int_distribution<int> ed(INT_MIN, INT_MAX);
        int data[4] {ed(*e__.get()), ed(*e__.get()), ed(*e__.get()), ed(*e__.get())};
        char c_str[40];
        sprintf(c_str,"%0X%0X%0X%0X",data[0],data[1],data[2],data[3]);
        std::string ret = c_str;
        ret.insert(ret.begin()+20,'-');
        ret.insert(ret.begin()+16,'-');
        ret.insert(ret.begin()+12,'-');
        ret.insert(ret.begin()+8,'-');
        return ret;
    }
}