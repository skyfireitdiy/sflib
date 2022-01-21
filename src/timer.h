
#pragma once

#include "nocopy.h"
#include "object.hpp"
#include "stdc++.h"

namespace skyfire
{
class timer : public nocopy<object>
{
public:
    sf_singal(timeout);

public:
    void start(int ms, bool once = false);
    void stop();
    bool is_active() const;

private:
    std::atomic<bool>            running__ { false };
    std::thread::id              current_timer_thread__;
    std::unique_ptr<std::thread> loop_thread__;
};
} // namespace skyfire
