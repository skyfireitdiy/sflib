#pragma once

#include "sf_stdc++.h"

namespace skyfire
{

struct co_ctx
{
    void*             regs[14] = {};
    bool              main;
    bool              running = false;
    std::vector<char> stack_data;
    size_t            stack_size;
    char*             sp;
    char*             bp;
    void (*entry)(void*);
    void* args;

    co_ctx(bool main = false, size_t ss = 2048)
        : main(main)
        , stack_data(ss)
        , stack_size(ss)
    {
        sp = stack_data.data();
    }
};

struct co_env : public std::enable_shared_from_this<co_env>
{
private:
    std::vector<std::shared_ptr<co_ctx>> co_set__;
    int                                  curr_co_index__ = -1;
    std::shared_ptr<co_ctx>              current_co__    = nullptr;

    std::shared_ptr<co_ctx> choose_co__();
    void                    append_co__(std::shared_ptr<co_ctx> ctx);

public:
    co_env();
    std::shared_ptr<co_ctx> choose_co();
    void                    release_curr_co();
    std::shared_ptr<co_ctx> get_current_coroutine() const;
    void                    run_coroutine(std::shared_ptr<co_ctx> ctx);
};

std::shared_ptr<co_ctx> create_coroutine(void (*func)(void*), void* args);
std::shared_ptr<co_ctx> get_current_coroutine();
void                    run_coroutine(std::shared_ptr<co_ctx> ctx);
void                    release_curr_co();
void                    yield_coroutine();

}