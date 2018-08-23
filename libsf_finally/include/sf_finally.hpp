//
// Created by skyfire on 2018/8/22 0022.
//
#pragma once

#include "sf_finally.h"

namespace skyfire
{
    inline sf_finally::sf_finally(std::function<void()> func) :func__(func)
    {
    }

    inline sf_finally::~sf_finally()
    {
        func__();
    }

}