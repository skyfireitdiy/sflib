#pragma once

#include <random>
#include <memory>

#include "sf_nocopy.hpp"
#include "sf_single_instance.hpp"

namespace skyfire {
    class sf_random : public sf_nocopy<> {
    public:
        SF_SINGLE_TON(sf_random)

    private:

        std::random_device rd__;
        std::shared_ptr<std::default_random_engine> e__;

        sf_random();

    public:
        int get_int(int min, int max);
        double get_double(double min, double max);
    };

}