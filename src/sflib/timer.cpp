
#include "sflib/timer.h"

namespace skyfire
{
void timer::start(int ms, bool once)
{
    if (running__)
    {
        return;
    }
    running__              = true;
    loop_thread__          = std::make_unique<std::thread>([this, ms](bool is_once) {
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));
            if (std::this_thread::get_id() != current_timer_thread__)
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
    current_timer_thread__ = loop_thread__->get_id();
}
bool timer::is_active() const { return running__; }
void timer::stop() { running__ = false; }
} // namespace skyfire
