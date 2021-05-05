
#include "coroutine.h"
#include "co_env.h"
#include "co_manager.h"
#include "error.h"

namespace skyfire
{

std::once_flag& get_co_once_flag()
{
    static std::once_flag flag;
    return flag;
}

void coroutine::wait()
{
    while (ctx__->get_state() != co_state::finished)
    {
        this_coroutine::yield_coroutine();
    }
}

bool coroutine::joinable() const
{
    return !(joined__);
}

void coroutine::join()
{
    if (joined__)
    {
        throw std::runtime_error("already joined");
    }
    joined__  = true;
    invalid__ = true;
    wait();
    delete ctx__;
}

bool coroutine::valid() const
{
    return !invalid__;
}

coroutine::~coroutine()
{
    if (ctx__ == nullptr)
    {
        return;
    }
    if (!joined__)
    {
        get_co_manager()->detached_co(ctx__);
    }
}

coroutine::coroutine(coroutine&& other)
    : ctx__(other.ctx__)
    , joined__(other.joined__)
    , invalid__(other.invalid__)
{
    other.ctx__     = nullptr;
    other.invalid__ = true;
}

coroutine& coroutine::operator=(coroutine&& other)
{
    ctx__     = other.ctx__;
    joined__  = other.joined__;
    invalid__ = other.invalid__;

    other.ctx__     = nullptr;
    other.invalid__ = true;

    return *this;
}

std::string this_coroutine::get_name()
{
    return get_co_env()->get_curr_co()->get_name();
}

void this_coroutine::yield_coroutine()
{
    get_co_env()->yield_coroutine();
}

long long this_coroutine::get_id()
{
    return get_co_env()->get_curr_co()->get_id();
}
}