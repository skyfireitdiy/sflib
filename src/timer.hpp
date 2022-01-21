
#pragma once
#include "cocpp/interface/co_this_co.h"
#include "timer.h"

using namespace cocpp;
namespace skyfire
{
inline void timer::start(int ms, bool once)
{
    if (running__)
    {
        return;
    }
    running__              = true;
    loop_co__              = std::make_unique<co>([this, ms](bool is_once) {
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));
            if (this_co::id() != current_timer_thread__)
            {
                return;
            }
            if (running__)
            {
                timeout();
                if (is_once)
                {
                    running__ = false;
                    return;
                }
            }
            else
            {
                break;
            }
        }
    },
                                     once);
    current_timer_thread__ = loop_co__->id();
}
inline bool timer::is_active() const { return running__; }
inline void timer::stop() { running__ = false; }
} // namespace skyfire
