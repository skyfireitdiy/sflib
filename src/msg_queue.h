
#pragma once
#include "co_condition_variable.h"
#include "co_mutex.h"
#include "single_instance.hpp"
#include "stdc++.h"


namespace skyfire
{
template <typename T>
class msg_queue
{
private:
    std::list<T>          data__;
    co_mutex              mu_data_op__;
    co_condition_variable wait_cond__;

public:
    void               add_msg(T msg);
    void               remove_msg(std::function<bool(const T&)> op);
    void               clear();
    std::shared_ptr<T> take_msg();
    std::list<T>       take_all_msg();
    bool               empty() const;
    void               wait_new_msg();
    void               wait_msg();
    void               add_empty_msg();
};
} // namespace skyfire
