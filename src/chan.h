
#pragma once
#include "error.h"
#include "logger.hpp"
#include "nocopy.h"
#include "utils.h"
#include <atomic>
#include <condition_variable>
#include <exception>
#include <mutex>
#include <queue>
namespace skyfire
{
template <typename T>
class chan : public make_instance_t<chan<T>, nocopy<>>
{
private:
    std::queue<T>           data__;
    size_t                  max_size__;
    std::atomic_bool        closed__ { false };
    std::mutex              mu__;
    std::condition_variable cond__;
    T                       buf__;
    std::atomic_bool        has_value__ { false };
    std::condition_variable cond_pop__;
    std::condition_variable cond_push_prepare__;
    std::condition_variable cond_pop_finish__;

public:
    explicit chan(size_t buffer_size = 0);
    void            close();
    friend sf_error operator>>(chan<T>& ch, T& d)
    {
        std::unique_lock<std::mutex> lck(ch.mu__);
        if (ch.closed__)
        {
            return sf_error { exception(err_chan_close, "channel already closed") };
        }
        if (ch.max_size__ != 0)
        {
            if (ch.data__.empty())
            {
                ch.cond__.wait(lck, [&ch]() { return ch.data__.size() > 0 || ch.closed__; });
                if (ch.closed__)
                {
                    throw exception(err_chan_close, "channel already closed");
                }
            }
            d = ch.data__.front();
            ch.data__.pop();
            ch.cond__.notify_all();
        }
        else
        {
            ch.cond_pop__.wait(lck, [&ch]() -> bool { return ch.has_value__; });
            d              = ch.buf__;
            ch.has_value__ = false;
            ch.cond_pop_finish__.notify_one();
        }
        return sf_error {};
    }
    friend sf_error operator>>(T d, chan<T>& ch)
    {
        std::unique_lock<std::mutex> lck(ch.mu__);
        if (ch.closed__)
        {
            return sf_error(exception(err_chan_close, "channel already closed"));
        }
        if (ch.max_size__ != 0)
        {
            if (ch.data__.size() == ch.max_size__)
            {
                ch.cond__.wait(lck, [&ch]() { return ch.data__.size() < ch.max_size__ || ch.closed__; });
                if (ch.closed__)
                {
                    throw exception(err_chan_close, "channel already closed");
                }
            }
            ch.data__.push(d);
            ch.cond__.notify_all();
        }
        else
        {
            if (ch.has_value__)
            {
                ch.cond_push_prepare__.wait(lck, [&ch] { return !ch.has_value__; });
                if (ch.closed__)
                {
                    throw exception(err_chan_close, "channel already closed");
                }
            }
            ch.buf__       = d;
            ch.has_value__ = true;
            ch.cond_pop__.notify_one();
            ch.cond_pop_finish__.wait(lck, [&ch] { return !ch.has_value__; });
            ch.cond_push_prepare__.notify_one();
        }
        return sf_error {};
    }
    friend sf_error operator>>(T d, const std::shared_ptr<chan<T>> ch)
    {
        return d >> *ch;
    }
    friend sf_error operator>>(const std::shared_ptr<chan<T>> ch, T& d)
    {
        return *ch >> d;
    }
};
}
