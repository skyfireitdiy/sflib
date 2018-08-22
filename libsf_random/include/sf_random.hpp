#pragma once

#include <random>
#include <memory>

#include "sf_nocopy.h"
#include "sf_single_instance.hpp"

namespace skyfire {
    class sf_random : public sf_nocopy<> {
    public:
        SF_SINGLE_TON(sf_random)

    private:

        std::random_device rd__;
        std::shared_ptr<std::default_random_engine> e__;

        sf_random() {
            e__ = std::make_shared<std::default_random_engine>(rd__());
        }

    public:
        int get_int(int min, int max) {
            std::uniform_int_distribution<int> ed(min, max);
            return ed(*e__.get());
        }
        double get_double(double min, double max){
            std::uniform_real_distribution<double > ed(min, max);
            return ed(*e__.get());
        }
    };
}