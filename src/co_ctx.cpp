
#include "co_ctx.h"
#include "co_env.h"
#include "co_manager.h"

#define __co_memory_barrier__() __asm volatile("" :: \
                                                   : "memory")

namespace skyfire
{
std::function<void()> co_ctx::get_entry() const
{
    return entry__;
}

void co_ctx::save_stack()
{
    if (state__ != co_state::finished)
    {
        saved_stack__.assign((char*)regs__[co_rspindex], stack_data__ + stack_size__);
    }
}

void co_ctx::restore_stack()
{
    memcpy(stack_data__ + default_co_stack_size - saved_stack__.size(), saved_stack__.data(), saved_stack__.size());
}

std::string co_ctx::get_name() const
{
    return name__;
}

long long co_ctx::get_id() const
{
    return reinterpret_cast<long>(this);
}

co_state co_ctx::get_state() const
{
    return state__;
}

void co_ctx::set_state(co_state state)
{
    state__ = state;
    get_co_manager()->one_co_finished();
}

void co_ctx::set_reg_ret(const void* value)
{
    regs__[co_retindex] = value;
}

void co_ctx::set_reg_rdi(const void* value)
{
    regs__[co_rdiindex] = value;
}

void co_ctx::set_reg_rsp(const void* value)
{
    regs__[co_rspindex] = value;
}

void co_ctx::set_reg_rbp(const void* value)
{
    regs__[co_rbpindex] = value;
}

const void* co_ctx::get_stack_bp() const
{
    return stack_data__ + stack_size__;
}

const void* co_ctx::get_reg_buf() const
{
    return &regs__;
}

bool co_ctx::shared_stack() const
{
    return shared_stack__;
}

void co_ctx::init()
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

co_ctx::co_ctx(std::function<void()> entry, const co_attr_config& attr)
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

co_ctx::~co_ctx()
{
    if (stack_data__ != nullptr && !shared_stack__)
    {
        delete[] stack_data__;
    }
}

void co_ctx::set_wait_cond()
{
    wait_cond__ = true;
}

void co_ctx::unset_wait_cond()
{
    wait_cond__ = false;
}

void co_ctx::wait_cond()
{
    while (wait_cond__)
    {
        get_co_env()->yield_coroutine();
    }
}

void co_ctx::wait_cond(std::function<bool()> cond)
{
    if (cond())
    {
        wait_cond__ = false;
        return;
    }
    while (wait_cond__)
    {
        if (cond())
        {
            wait_cond__ = false;
            return;
        }
        get_co_env()->yield_coroutine();
    }
}

void __switch_co__(co_ctx* curr, co_ctx* next)
{
    if (curr->get_state() == co_state::running)
    {
        curr->set_state(co_state::suspended);
    }
    next->set_state(co_state::running);
    __co_memory_barrier__();
    __swap_regs__(curr->get_reg_buf(), next->get_reg_buf());
    __co_memory_barrier__();
}

/*| 64 bit
 *|-------------------------------------------------|
 *| low  | regs[0]: r15                 |  0         |            
 *|      | regs[1]: r14                 |  8         |            
 *|      | regs[2]: r13                 |  16        |               
 *|      | regs[3]: r12                 |  24        |         
 *|      | regs[4]: rbp                 |  32        |        
 *|      | regs[5]: rip(ret)            |  40        |         
 *|      | regs[6]: rbx                 |  48        |           
 *|      | regs[7]: rsp                 |  56        |
 *|      | regs[8]: rdi                 |  64        |       
 *| high | regs[9]: fc_mxcsr|fc_x87_cw |  72/76     |          
 *|-------------------------------------------------|
 */

void __swap_regs__(const void*, const void*)
{
    __asm("popq %rbp\n");
    __asm("movq %r15, (%rdi)\n");
    __asm("movq %r14, 8(%rdi)\n");
    __asm("movq %r13, 16(%rdi)\n");
    __asm("movq %r12, 24(%rdi)\n");
    __asm("movq %rbp, 32(%rdi)\n");
    __asm("movq 0(%rsp), %rax\n");
    __asm("movq %rax, 40(%rdi)\n");
    __asm("movq %rbx, 48(%rdi)\n");
    __asm("movq %rsp, 56(%rdi)\n");
    __asm("movq %rdi, 64(%rdi)\n");
    __asm("movq 64(%rsi), %rdi\n");
    __asm("movq 56(%rsi), %rsp\n");
    __asm("movq 48(%rsi), %rbx\n");
    __asm("movq 40(%rsi), %rax\n");
    __asm("movq %rax, 0(%rsp)\n");
    __asm("movq 32(%rsi), %rbp\n");
    __asm("movq 24(%rsi), %r12\n");
    __asm("movq 16(%rsi), %r11\n");
    __asm("movq 8(%rsi), %r10\n");
    __asm("movq (%rsi), %r9\n");
    __asm("pushq %rbp\n");
}

void __co_func__(co_ctx* ctx)
{
    ctx->get_entry()();
    get_co_env()->release_curr_co();
}

void __co_save_stack__()
{
    for (;;)
    {
        auto prev = get_co_env()->get_prev_co();
        if (prev->shared_stack())
        {
            prev->save_stack();
        }
        auto next = get_co_env()->get_next_co();
        if (next->shared_stack())
        {
            next->restore_stack();
        }
        auto curr = get_co_env()->get_curr_co();
        get_co_env()->set_curr_co(next);
        __switch_co__(curr, next);
    }
}
}