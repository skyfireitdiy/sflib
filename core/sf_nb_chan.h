
/**
* @version 1.0.0
* @author skyfire
* @file sf_nb_chan.h

* 
*/

#pragma once
#include "tools/sf_nocopy.h"
#include "tools/sf_utils.h"
#include "core/sf_stdc++.h"
#include "core/sf_chan_utils.h"

namespace skyfire{

    /**
     * 无缓冲管道
     * @tparam T 管道类型
     */
    template<typename T>
    class sf_nb_chan: public sf_make_instance_t<sf_nb_chan<T>, sf_nocopy<>>
    {
    private:
        T buf__;
        std::mutex mu__;
        std::atomic_bool has_value__ {false};
        std::atomic_bool closed__ {false};
        std::condition_variable cond_pop__;
        std::condition_variable cond_push_prepare__;
        std::condition_variable cond_pop_finish__;

    public:

        /**
         * 关闭管道
         */
        void close();

        /**
         * 读取管道
         * @param d 读取的数据
         * @param ch 管道
         */
        friend void operator<<(T &d, sf_nb_chan<T>& ch) {
            if (ch.closed__) {
                throw sf_chan_close_exception();
            }
            std::unique_lock<std::mutex> lck(ch.mu__);
            ch.cond_pop__.wait(lck, [&ch]()->bool{return ch.has_value__;});
            d = ch.buf__;
            ch.has_value__ = false;
            ch.cond_pop_finish__.notify_one();
        }

        /**
         * 写入管道
         * @param ch 管道
         * @param d 写入的数据
         */
        friend void operator<<(sf_nb_chan<T> &ch, T d){
            if(ch.closed__){
                throw sf_chan_close_exception();
            }
            std::unique_lock<std::mutex> lck(ch.mu__);
            if(ch.has_value__){
                ch.cond_push_prepare__.wait(lck, [&ch]{return !ch.has_value__;});
                if (ch.closed__) {
                    throw sf_chan_close_exception();
                }
            }
            ch.buf__ = d;
            ch.has_value__ = true;
            ch.cond_pop__.notify_one();
            ch.cond_pop_finish__.wait(lck, [&ch]{return !ch.has_value__;});
            ch.cond_push_prepare__.notify_one();
        }

        friend void operator<<(const std::shared_ptr<sf_nb_chan<T>> &ch, T d){
            *ch<<d;
        }

        friend void operator<<(T &d, const std::shared_ptr<sf_nb_chan<T>> &ch){
            d<<*ch;
        }
    };
}