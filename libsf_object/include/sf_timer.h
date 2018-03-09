#pragma once

#include "sf_nocopy.h"
#include "sf_object.h"
#include <atomic>

namespace skyfire
{
    class sf_timer : public sf_nocopy<sf_object<>>
    {
    public:
        SF_REG_SIGNAL(timeout);
    public:
        void start(int ms, bool once = false);
        void stop();
        bool is_active();

    private:
        std::atomic<bool> running__ {false};
    };
}