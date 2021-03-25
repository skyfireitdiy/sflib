#pragma once
#include "co_attr.h"
#include "stdc++.h"

namespace skyfire
{

constexpr int co_retindex = 9;
constexpr int co_rspindex = 13;
constexpr int co_rdiindex = 7;
constexpr int co_rbpindex = 6;

//-------------
// 64 bit
// low | regs[0]: r15 |
//    | regs[1]: r14 |
//    | regs[2]: r13 |
//    | regs[3]: r12 |
//    | regs[4]: r9  |
//    | regs[5]: r8  |
//    | regs[6]: rbp |
//    | regs[7]: rdi |
//    | regs[8]: rsi |
//    | regs[9]: ret |  //ret func addr
//    | regs[10]: rdx |
//    | regs[11]: rcx |
//    | regs[12]: rbx |
// hig | regs[13]: rsp |

enum class co_state
{
    ready,
    running,
    suspended,
    finished
};

class co_ctx final
{
    const void*           regs__[14]   = {};
    co_state              state__      = co_state::ready;
    char*                 stack_data__ = nullptr;
    size_t                stack_size__;
    std::function<void()> entry__;
    bool                  detached__;
    mutable std::mutex    mu_detached__;
    bool                  shared_stack__;
    std::vector<char>     saved_stack__;
    std::string           name__;

public:
    co_ctx(std::function<void()> entry, const co_attr_config& attr);

    std::function<void()> get_entry() const;
    ~co_ctx();

    co_state    get_state() const;
    void        init();
    void        set_state(co_state state);
    void        set_reg_ret(const void* value);
    void        set_reg_rdi(const void* value);
    void        set_reg_rsp(const void* value);
    void        set_reg_rbp(const void* value);
    const void* get_stack_bp() const;
    const void* get_reg_buf() const;
    void        set_detached();
    bool        detached() const;
    bool        shared_stack() const;
    void        save_stack();
    void        restore_stack();
    void        set_curr_sp(char* sp);
    std::string get_name() const;
    long long   get_id() const;
};

}