
#include "timer.h"
namespace skyfire
{
void timer::start(int ms, bool once)
{
    if (running__)
    {
        return;
    }
    running__              = true;
    loop_co__              = std::make_unique<coroutine>([this, ms](bool is_once) {
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));
            if (this_coroutine::get_id() != current_timer_thread__)
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
    current_timer_thread__ = loop_co__->get_id();
}
bool timer::is_active() const { return running__; }
void timer::stop() { running__ = false; }
} // namespace skyfire
