
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_random.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#include "sf_random.h"

namespace skyfire {
    inline sf_random::sf_random() {
        e__ = std::make_shared<std::default_random_engine>(rd__());
    }

    inline int sf_random::get_int(const int min, const int max)  const
    {
	    const std::uniform_int_distribution<int> ed(min, max);
        return ed(*e__.get());
    }

    inline double sf_random::get_double(const double min, const double max) const
    {
	    const std::uniform_real_distribution<double > ed(min, max);
        return ed(*e__.get());
    }

    inline std::string sf_random::get_uuid_str() const
    {
	    const std::uniform_int_distribution<int> ed(INT_MIN, INT_MAX);
        int data[4] {ed(*e__.get()), ed(*e__.get()), ed(*e__.get()), ed(*e__.get())};
        char c_str[40];
#ifdef _MSC_VER
		sprintf_s(c_str, "%0X%0X%0X%0X", data[0], data[1], data[2], data[3]);
#else
		sprintf(c_str, "%0X%0X%0X%0X", data[0], data[1], data[2], data[3]);
#endif // _MSC_VER
        std::string ret = c_str;
        ret.insert(ret.begin()+20,'-');
        ret.insert(ret.begin()+16,'-');
        ret.insert(ret.begin()+12,'-');
        ret.insert(ret.begin()+8,'-');
        return ret;
    }
}