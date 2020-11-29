
/**
* @version 1.0.0
* @author skyfire
* @file sf_chan.h

* 
*/

#pragma once

#include <atomic>
#include <condition_variable>
#include <exception>
#include <mutex>
#include <queue>

#include "sf_error.h"
#include "sf_nocopy.h"
#include "sf_utils.h"

#include "sf_logger.hpp"

namespace skyfire
{

/**
 * 管道（简化线程间通信）
 * @tparam T 管道类型
 */
template <typename T>
class chan : public make_instance_t<chan<T>, nocopy<>>
{
private:
    std::queue<T>           data__;
    int                     max_size__;
    std::atomic_bool        closed__ { false };
    std::mutex              mu__;
    std::condition_variable cond__;

    T                       buf__;
    std::atomic_bool        has_value__ { false };
    std::condition_variable cond_pop__;
    std::condition_variable cond_push_prepare__;
    std::condition_variable cond_pop_finish__;

public:
    /**
     * 构造一个缓冲区大小为buffer_size的管道
     * @param buffer_size 缓冲区大小
     */
    explicit chan(int buffer_size = 0);

    /**
     * 关闭管道
     */
    void close();

    /**
     * 读取管道
     * @param d 读取的数据
     * @param ch 管道
     */
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

    /**
     * 写入管道
     * @param ch 管道
     * @param d 写入的数据
     */
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
