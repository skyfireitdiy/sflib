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
}