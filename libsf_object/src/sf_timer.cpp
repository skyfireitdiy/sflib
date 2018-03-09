#include "sf_timer.h"
#include <chrono>

namespace skyfire
{
    void sf_timer::start(int ms, bool once)
    {
        if(running__)
        {
            return;
        }
        running__ = true;
        while(true)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));
            if(running__)
            {
                timeout();
                if(once)
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
        running__ = false;
    }

    void sf_timer::stop()
    {
        running__ = false;
    }

    bool sf_timer::is_active()
    {
        return running__;
    }
}