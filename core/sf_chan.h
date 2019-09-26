#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <atomic>
#include <exception>

#include "tools/sf_utils.h"
#include "tools/sf_nocopy.h"
#include "core/sf_chan_utils.h"


namespace skyfire{

    template <typename T>
    class sf_chan: public sf_make_instance_t<sf_chan<T>,sf_nocopy<>>{
    private:
        std::queue<T> data__;
        int max_size__;
        std::atomic_bool closed__{false};
        std::mutex mu__;
        std::condition_variable cond__;
    public:


        explicit sf_chan(int buffer_size);

        void close();

        friend void operator<<(T &d, sf_chan<T>& ch){
            if(ch.closed__){
                throw sf_chan_close_exception();
            }
            std::unique_lock<std::mutex> lck(ch.mu__);
            if(ch.data__.empty()){
                ch.cond__.wait(lck, [&ch](){return ch.data__.size() > 0 || ch.closed__;});
                if(ch.closed__){
                    throw sf_chan_close_exception();
                }
            }
            d = ch.data__.front();
            ch.data__.pop();
            ch.cond__.notify_all();
        }


        friend void operator<<(sf_chan<T>& ch, T d){
            if(ch.closed__){
                throw sf_chan_close_exception();
            }
            std::unique_lock<std::mutex> lck(ch.mu__);
            if(ch.data__.size()==ch.max_size__){
                ch.cond__.wait(lck, [&ch](){ return ch.data__.size() < ch.max_size__ || ch.closed__;});
                if(ch.closed__){
                    throw sf_chan_close_exception();
                }
            }
            ch.data__.push(d);
            ch.cond__.notify_all();
        }


        friend void operator<<(T &d, const std::shared_ptr<sf_chan<T>> ch){
            d<<*ch;
        }

        friend void operator<<(const std::shared_ptr<sf_chan<T>> ch, T d){
            *ch<<d;
        }
    };

}

