
#pragma once
#include "cocpp/interface/co.h"
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
    std::atomic<bool>   running__ { false };
    unsigned long long  current_timer_thread__;
    std::unique_ptr<co> loop_co__;
};
} // namespace skyfire
