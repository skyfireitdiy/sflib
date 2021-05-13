
#pragma once
#include "co_condition_variable.h"
#include "co_mutex.h"
#include "error.h"
#include "logger.h"
#include "nocopy.h"
#include "utils.h"
#include <bits/stdc++.h>

namespace skyfire
{
template <typename T>
class chan : public instance_maker<chan<T>, nocopy<>>
{
private:
    std::queue<T>         data__;
    size_t                max_size__;
    std::atomic_bool      closed__ { false };
    co_mutex              mu__;
    co_condition_variable cond__;
    T                     buf__;
    std::atomic_bool      has_value__ { false };
    co_condition_variable cond_pop__;
    co_condition_variable cond_push_prepare__;
    co_condition_variable cond_pop_finish__;

public:
    explicit chan(size_t buffer_size = 0);
    void            close();
    friend sf_error operator>>(chan<T>& ch, T& d)
    {
        std::unique_lock<co_mutex> lck(ch.mu__);
        if (ch.closed__)
        {
            return sf_error { exception(err_chan_close, "channel already closed") };
        }
        if (ch.max_size__ != 0)
        {
            if (ch.data__.empty())
            {
                ch.cond__.wait(lck, std::function<bool()>([&ch]() { return ch.data__.size() > 0 || ch.closed__; }));
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
        std::unique_lock<co_mutex> lck(ch.mu__);
        if (ch.closed__)
        {
            return sf_error(exception(err_chan_close, "channel already closed"));
        }
        if (ch.max_size__ != 0)
        {
            if (ch.data__.size() == ch.max_size__)
            {
                ch.cond__.wait(lck, std::function<bool()>([&ch]() { return ch.data__.size() < ch.max_size__ || ch.closed__; }));
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

//impl

template <typename T>
chan<T>::chan(size_t buffer_size)
{
    max_size__ = buffer_size;
}
template <typename T>
void chan<T>::close()
{
    std::unique_lock<co_mutex> lck(mu__);
    closed__ = true;
    // FIXME deque没有clear成员了？
    // data__.clear();
    data__ = std::queue<T> {};
    if (max_size__ != 0)
    {
        cond__.notify_all();
    }
    else
    {
        cond_pop__.notify_all();
    }
}
}
