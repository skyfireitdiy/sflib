#pragma once

#include "sf_nocopy.h"
#include "sf_object.h"
#include <atomic>

namespace skyfire
{
    class sf_timer : public sf_nocopy<sf_object>, public std::enable_shared_from_this<sf_timer>
    {
    public:
        SF_REG_SIGNAL(timeout);
    public:
        void start(int ms, bool once = false)
        {
            if(running__)
            {
                return;
            }
            running__ = true;
            std::thread([=]()
                        {
                            while (true)
                            {
                                std::this_thread::sleep_for(std::chrono::milliseconds(ms));
                                if (running__)
                                {
                                    timeout();
                                    if (once)
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
                        }).detach();
        }


        void stop()
        {
            running__ = false;
        }

        bool is_active()
        {
            return running__;
        }

    private:
        std::atomic<bool> running__ {false};
    };
}