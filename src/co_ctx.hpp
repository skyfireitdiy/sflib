#pragma once

#include "co_ctx.h"
#include "co_env.h"
#include "co_utils.h"

namespace skyfire
{
inline std::function<void()> co_ctx::get_entry() const
{
    return entry__;
}

inline void co_ctx::save_stack()
{
    if (state__ != co_state::finished)
    {
        saved_stack__.assign((char*)regs__[co_rspindex], stack_data__ + stack_size__);
    }
}

inline void co_ctx::restore_stack()
{
    memcpy(stack_data__ + default_co_stack_size - saved_stack__.size(), saved_stack__.data(), saved_stack__.size());
}

inline std::string co_ctx::get_name() const
{
    return name__;
}

inline long long co_ctx::get_id() const
{
    return reinterpret_cast<long>(this);
}

inline co_state co_ctx::get_state() const
{
    return state__;
}

inline void co_ctx::set_state(co_state state)
{
    state__ = state;
}

inline void co_ctx::set_reg_ret(const void* value)
{
    regs__[co_retindex] = value;
}

inline void co_ctx::set_reg_rdi(const void* value)
{
    regs__[co_rdiindex] = value;
}

inline void co_ctx::set_reg_rsp(const void* value)
{
    regs__[co_rspindex] = value;
}

inline void co_ctx::set_reg_rbp(const void* value)
{
    regs__[co_rbpindex] = value;
}

inline const void* co_ctx::get_stack_bp() const
{
    return stack_data__ + stack_size__;
}

inline const void* co_ctx::get_reg_buf() const
{
    return &regs__;
}

inline bool co_ctx::shared_stack() const
{
    return shared_stack__;
}

inline void co_ctx::set_detached()
{
    std::lock_guard<std::mutex> lck(mu_detached__);
    detached__ = true;
}

inline bool co_ctx::detached() const
{
    std::lock_guard<std::mutex> lck(mu_detached__);
    return detached__;
}

inline void co_ctx::init()
{
    set_reg_ret(reinterpret_cast<void*>(&__co_func__));
    set_reg_rdi(reinterpret_cast<void*>(this));
    set_reg_rbp(const_cast<void*>(get_stack_bp()));
    const void* sp  = reinterpret_cast<const char*>(get_stack_bp()) - sizeof(void*);
    sp              = reinterpret_cast<char*>((unsigned long)sp & -16LL);
    void** ret_addr = (void**)(sp);
    *ret_addr       = reinterpret_cast<void*>(&__co_func__);
    set_reg_rsp(reinterpret_cast<const char*>(sp) - sizeof(void*) * 2);
    if (shared_stack__)
    {
        save_stack();
    }
}

inline co_ctx::co_ctx(std::function<void()> entry, const co_attr_config& attr)
    : stack_size__(attr.stack_size)
    , entry__(entry)
    , shared_stack__(attr.shared_stack)
    , name__(attr.name)
{
    if (stack_size__ != 0 && !shared_stack__)
    {
        stack_data__ = new char[stack_size__];
    }
    if (shared_stack__)
    {
        stack_size__ = default_co_stack_size;
        stack_data__ = get_co_env()->get_shared_stack();
    }
    if (stack_size__ != 0 && entry != nullptr)
    {
        init();
    }
}

inline co_ctx::~co_ctx()
{
    if (stack_data__ != nullptr && !shared_stack__)
    {
        delete[] stack_data__;
    }
}

inline void co_ctx::set_wait_cond()
{
    wait_cond__ = true;
}

inline void co_ctx::unset_wait_cond() 
{
    wait_cond__ = false;
}

inline void co_ctx::wait_cond()
{
    while(wait_cond__){
        this_coroutine::yield_coroutine();
    }
}

}